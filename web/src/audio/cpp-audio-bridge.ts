// Bridge between C++ audio calls and Web Audio API
// This implements window.ToonTalkAudio that C++ calls via EM_ASM

export class CppAudioBridge {
    private audioContext: AudioContext;
    private sounds: Map<string, AudioBuffer> = new Map();
    private activeSources: Map<string, AudioBufferSourceNode> = new Map();

    constructor() {
        this.audioContext = new AudioContext();

        // Expose this to C++ via global window object
        (window as any).ToonTalkAudio = this;
    }

    /**
     * Play a sound (called from C++ via EM_ASM)
     * @param soundName - Name of the sound file (without extension)
     * @param loop - Whether to loop the sound
     */
    async playSound(soundName: string, loop: boolean = false): Promise<void> {
        try {
            // Get or load audio buffer
            let buffer = this.sounds.get(soundName);
            if (!buffer) {
                buffer = await this.loadSound(soundName);
                if (!buffer) {
                    console.error(`Failed to load sound: ${soundName}`);
                    return;
                }
            }

            // Stop existing instance if playing
            this.stopSound(soundName);

            // Create source
            const source = this.audioContext.createBufferSource();
            source.buffer = buffer;
            source.loop = loop;
            source.connect(this.audioContext.destination);

            // Track active source
            this.activeSources.set(soundName, source);

            // Remove from active when done
            source.onended = () => {
                this.activeSources.delete(soundName);
            };

            // Start playback
            source.start(0);
        } catch (error) {
            console.error(`Error playing sound ${soundName}:`, error);
        }
    }

    /**
     * Stop a sound (called from C++ via EM_ASM)
     */
    stopSound(soundName: string): void {
        const source = this.activeSources.get(soundName);
        if (source) {
            try {
                source.stop();
            } catch (error) {
                // Already stopped
            }
            this.activeSources.delete(soundName);
        }
    }

    /**
     * Set volume for a sound (called from C++ via EM_ASM)
     */
    setVolume(soundName: string, volume: number): void {
        const source = this.activeSources.get(soundName);
        if (source) {
            // Create gain node for volume control
            const gainNode = this.audioContext.createGain();
            gainNode.gain.value = volume;
            source.disconnect();
            source.connect(gainNode);
            gainNode.connect(this.audioContext.destination);
        }
    }

    /**
     * Load a sound file
     */
    private async loadSound(soundName: string): Promise<AudioBuffer | null> {
        try {
            // Try different audio formats
            const formats = ['wav', 'mp3', 'ogg'];

            for (const format of formats) {
                const url = `/assets/sounds/${soundName}.${format}`;

                try {
                    const response = await fetch(url);
                    if (!response.ok) continue;

                    const arrayBuffer = await response.arrayBuffer();
                    const audioBuffer = await this.audioContext.decodeAudioData(arrayBuffer);

                    // Cache it
                    this.sounds.set(soundName, audioBuffer);
                    return audioBuffer;
                } catch (error) {
                    // Try next format
                    continue;
                }
            }

            console.error(`Sound not found in any format: ${soundName}`);
            return null;
        } catch (error) {
            console.error(`Error loading sound ${soundName}:`, error);
            return null;
        }
    }

    /**
     * Preload common sounds
     */
    async preloadSounds(): Promise<void> {
        const soundsToLoad = [
            'pickup',
            'drop',
            'click',
            'whoosh',
            'beep',
            // Add more sound names as discovered from original code
        ];

        await Promise.all(
            soundsToLoad.map(name => this.loadSound(name))
        );
    }
}
