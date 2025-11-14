// Bridge between C++ rendering calls and PixiJS
// This implements window.ToonTalkRenderer that C++ calls via EM_ASM

import * as PIXI from 'pixi.js';

export class CppRendererBridge {
    private app: PIXI.Application;
    private container: PIXI.Container;
    private textureCache: Map<number, PIXI.Texture> = new Map();
    private spriteCache: Map<number, PIXI.Sprite> = new Map();

    constructor(app: PIXI.Application) {
        this.app = app;
        this.container = new PIXI.Container();
        this.app.stage.addChild(this.container);

        // Expose this to C++ via global window object
        (window as any).ToonTalkRenderer = this;
    }

    /**
     * Draw a sprite (called from C++ via EM_ASM)
     * @param spriteId - ID of the sprite from SpriteCode enum
     * @param x - X position
     * @param y - Y position
     * @param width - Width to render
     * @param height - Height to render
     * @param rotation - Rotation in degrees
     * @param alpha - Alpha transparency (0-1)
     */
    drawSprite(spriteId: number, x: number, y: number, width: number, height: number,
               rotation: number = 0, alpha: number = 1.0): void {
        // Get or create sprite
        let sprite = this.spriteCache.get(spriteId);

        if (!sprite) {
            const texture = this.getTextureForSpriteId(spriteId);
            if (!texture) {
                console.warn(`Texture not found for sprite ID: ${spriteId}`);
                return;
            }

            sprite = new PIXI.Sprite(texture);
            this.spriteCache.set(spriteId, sprite);
            this.container.addChild(sprite);
        }

        // Update sprite properties
        sprite.x = x;
        sprite.y = y;
        sprite.width = width;
        sprite.height = height;
        sprite.rotation = (rotation * Math.PI) / 180;
        sprite.alpha = alpha;
        sprite.visible = true;
    }

    /**
     * Draw a rectangle (called from C++ via EM_ASM)
     */
    drawRect(x: number, y: number, width: number, height: number, color: number): void {
        const graphics = new PIXI.Graphics();
        graphics.beginFill(color);
        graphics.drawRect(x, y, width, height);
        graphics.endFill();
        this.container.addChild(graphics);
    }

    /**
     * Draw a circle (called from C++ via EM_ASM)
     */
    drawCircle(x: number, y: number, radius: number, color: number): void {
        const graphics = new PIXI.Graphics();
        graphics.beginFill(color);
        graphics.drawCircle(x, y, radius);
        graphics.endFill();
        this.container.addChild(graphics);
    }

    /**
     * Draw a line (called from C++ via EM_ASM)
     */
    drawLine(x1: number, y1: number, x2: number, y2: number, color: number): void {
        const graphics = new PIXI.Graphics();
        graphics.lineStyle(2, color);
        graphics.moveTo(x1, y1);
        graphics.lineTo(x2, y2);
        this.container.addChild(graphics);
    }

    /**
     * Draw text (called from C++ via EM_ASM)
     */
    drawText(text: string, x: number, y: number, fontSize: number,
             color: number, fontFamily: string = 'Arial'): void {
        const pixiText = new PIXI.Text(text, {
            fontFamily,
            fontSize,
            fill: color,
        });
        pixiText.x = x;
        pixiText.y = y;
        this.container.addChild(pixiText);
    }

    /**
     * Clear the screen (called from C++ via EM_ASM)
     */
    clear(): void {
        // Remove all children
        this.container.removeChildren();

        // Reset sprite visibility (keep them cached)
        this.spriteCache.forEach(sprite => {
            sprite.visible = false;
        });
    }

    /**
     * Present/render the frame (called from C++ via EM_ASM)
     */
    present(): void {
        // PixiJS automatically renders, so this is mostly a no-op
        // But we could use it for timing/performance tracking
    }

    /**
     * Set clipping rectangle (called from C++ via EM_ASM)
     */
    setClipRect(x: number, y: number, width: number, height: number): void {
        const mask = new PIXI.Graphics();
        mask.beginFill(0xffffff);
        mask.drawRect(x, y, width, height);
        mask.endFill();
        this.container.mask = mask;
    }

    /**
     * Clear clipping (called from C++ via EM_ASM)
     */
    clearClip(): void {
        this.container.mask = null;
    }

    /**
     * Load texture for a sprite ID
     * Maps C++ SpriteCode enum to texture files
     */
    private getTextureForSpriteId(spriteId: number): PIXI.Texture | null {
        // Check cache first
        if (this.textureCache.has(spriteId)) {
            return this.textureCache.get(spriteId)!;
        }

        // Map sprite IDs to texture paths
        // These correspond to SpriteCode enum in C++
        const textureMap: { [key: number]: string } = {
            // NUMBER_PAD_SPRITE = 11 (from sprite.h)
            11: '/assets/M25/number.bmp',

            // TEXT_PAD_SPRITE = 25 (from sprite.h)
            25: '/assets/M25/text.bmp',

            // CUBBY_SPRITE = 14 (from sprite.h)
            14: '/assets/M25/box.bmp',

            // SCALE_SPRITE = 12 (from sprite.h)
            12: '/assets/M25/scale.bmp',

            // ROBOT_SPRITE = 19 (from sprite.h)
            19: '/assets/M25/robot.bmp',

            // Add more mappings as needed
        };

        const texturePath = textureMap[spriteId];
        if (!texturePath) {
            console.warn(`No texture mapping for sprite ID: ${spriteId}`);
            return null;
        }

        // Load texture
        try {
            const texture = PIXI.Texture.from(texturePath);
            this.textureCache.set(spriteId, texture);
            return texture;
        } catch (error) {
            console.error(`Failed to load texture: ${texturePath}`, error);
            return null;
        }
    }

    /**
     * Preload all textures
     */
    async preloadTextures(): Promise<void> {
        const texturesToLoad = [
            '/assets/M25/number.bmp',
            '/assets/M25/text.bmp',
            '/assets/M25/box.bmp',
            '/assets/M25/scale.bmp',
            '/assets/M25/robot.bmp',
        ];

        await Promise.all(
            texturesToLoad.map(path => PIXI.Assets.load(path))
        );
    }
}
