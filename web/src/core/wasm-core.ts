import type { ToonTalkCoreModule, CreateToonTalkCore } from '../types/wasm';

/**
 * WasmCore - Manages the ToonTalk WASM module
 *
 * Loads and initializes the C++ core compiled to WebAssembly
 */
export class WasmCore {
    private module?: ToonTalkCoreModule;
    private loaded: boolean = false;

    async initialize(): Promise<void> {
        console.log('[WASM] Loading ToonTalk core module...');

        try {
            // Import the WASM module
            // @ts-ignore - Vite will handle this import
            const createModule = (await import('../../core/build/toontalk-core.js')).default as CreateToonTalkCore;

            // Initialize the module
            this.module = await createModule({
                locateFile: (path: string) => {
                    // Tell Emscripten where to find the .wasm file
                    if (path.endsWith('.wasm')) {
                        return '/core/build/' + path;
                    }
                    return path;
                },
                print: (text: string) => {
                    console.log('[WASM stdout]', text);
                },
                printErr: (text: string) => {
                    console.error('[WASM stderr]', text);
                }
            });

            this.loaded = true;
            console.log('[WASM] Module loaded successfully!');

            // Test the module
            this.runTests();

        } catch (error) {
            console.error('[WASM] Failed to load module:', error);
            throw error;
        }
    }

    isLoaded(): boolean {
        return this.loaded;
    }

    getModule(): ToonTalkCoreModule | undefined {
        return this.module;
    }

    /**
     * Test WASM functions to verify everything works
     */
    private runTests(): void {
        if (!this.module) return;

        console.log('[WASM Tests] Running verification tests...');

        // Test 1: Version
        const version = this.module.getVersion();
        console.log('[WASM Test] Version:', version);

        // Test 2: Math function
        const sum = this.module.add(42, 13);
        console.log('[WASM Test] add(42, 13) =', sum);

        // Test 3: String function
        const greeting = this.module.greet('ToonTalk');
        console.log('[WASM Test] greet("ToonTalk"):', greeting);

        // Test 4: Create a Sprite
        const sprite = new this.module.Sprite(100, 100, 50, 50);
        console.log('[WASM Test] Created Sprite at:', sprite.getX(), sprite.getY());
        sprite.setPosition(200, 150);
        console.log('[WASM Test] Moved Sprite to:', sprite.getX(), sprite.getY());
        sprite.delete(); // Important: clean up C++ objects

        // Test 5: Create a Bird
        const bird = new this.module.Bird(300, 200);
        console.log('[WASM Test] Created Bird at:', bird.getX(), bird.getY());
        bird.setVelocity(5, -2);
        console.log('[WASM Test] Bird velocity set');
        bird.delete(); // Important: clean up

        console.log('[WASM Tests] ✅ All tests passed!');
    }

    /**
     * Create a Sprite instance from WASM
     */
    createSprite(x: number, y: number, width: number, height: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Sprite(x, y, width, height);
    }

    /**
     * Create a Bird instance from WASM
     */
    createBird(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Bird(x, y);
    }

    /**
     * Initialize the core system
     */
    initializeCore(): boolean {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return this.module.initialize();
    }
}

// Singleton instance
let wasmCoreInstance: WasmCore | null = null;

/**
 * Get the WASM core singleton instance
 */
export function getWasmCore(): WasmCore {
    if (!wasmCoreInstance) {
        wasmCoreInstance = new WasmCore();
    }
    return wasmCoreInstance;
}
