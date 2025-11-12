import * as PIXI from 'pixi.js';

/**
 * TextureManager - Loads and manages textures for ToonTalk objects
 *
 * This loads the original ToonTalk bitmap assets and provides them to sprites.
 * Textures are loaded once and shared across all sprite instances.
 */
export class TextureManager {
    private static instance: TextureManager;
    private textures: Map<string, PIXI.Texture> = new Map();
    private loadingPromise: Promise<void> | null = null;
    private loaded: boolean = false;

    private constructor() {}

    static getInstance(): TextureManager {
        if (!TextureManager.instance) {
            TextureManager.instance = new TextureManager();
        }
        return TextureManager.instance;
    }

    /**
     * Preload all ToonTalk textures
     */
    async loadTextures(): Promise<void> {
        if (this.loaded) return;
        if (this.loadingPromise) return this.loadingPromise;

        this.loadingPromise = this._loadAllTextures();
        await this.loadingPromise;
        this.loaded = true;
    }

    private async _loadAllTextures(): Promise<void> {
        console.log('[TextureManager] Loading original ToonTalk graphics...');

        const textureMap = {
            'box': '/assets/sprites/box.bmp',
            'nest': '/assets/sprites/nest.bmp',
            'bird': '/assets/sprites/pumpy.bmp',
            'number': '/assets/sprites/texpad.bmp',
            'text': '/assets/sprites/texpad.bmp', // Same as number in original
            'robot': '/assets/sprites/robot.bmp',
            'background': '/assets/backgrounds/city.bmp'
        };

        const loadPromises: Promise<void>[] = [];

        for (const [key, path] of Object.entries(textureMap)) {
            const promise = PIXI.Assets.load(path).then((texture: PIXI.Texture) => {
                this.textures.set(key, texture);
                console.log(`[TextureManager] ✓ Loaded ${key} (${path})`);
            }).catch((error: Error) => {
                console.warn(`[TextureManager] ⚠️  Failed to load ${key}: ${error.message}`);
                // Create fallback colored texture
                const graphics = new PIXI.Graphics();
                graphics.beginFill(this._getFallbackColor(key));
                graphics.drawRect(0, 0, 64, 64);
                graphics.endFill();
                const fallbackTexture = PIXI.RenderTexture.create({ width: 64, height: 64 });
                const renderer = PIXI.autoDetectRenderer();
                if (renderer instanceof PIXI.Renderer) {
                    renderer.render(graphics, { renderTexture: fallbackTexture });
                }
                this.textures.set(key, fallbackTexture);
            });
            loadPromises.push(promise);
        }

        await Promise.all(loadPromises);
        console.log('[TextureManager] ✅ All textures loaded');
    }

    /**
     * Get texture for a specific object type
     */
    getTexture(type: string): PIXI.Texture | undefined {
        return this.textures.get(type);
    }

    /**
     * Check if textures are loaded
     */
    isLoaded(): boolean {
        return this.loaded;
    }

    /**
     * Get fallback color for object type (if texture fails to load)
     */
    private _getFallbackColor(type: string): number {
        const colors: { [key: string]: number } = {
            'bird': 0xFF69B4,      // Pink (Pumpy)
            'number': 0xFFFF00,    // Yellow
            'text': 0xFFFFFF,      // White
            'box': 0x8B4513,       // Brown
            'nest': 0xDEB887,      // Tan
            'robot': 0x00FF00,     // Green
            'background': 0x87CEEB // Sky blue
        };
        return colors[type] || 0xCCCCCC;
    }
}

/**
 * Global accessor for texture manager
 */
export function getTextureManager(): TextureManager {
    return TextureManager.getInstance();
}
