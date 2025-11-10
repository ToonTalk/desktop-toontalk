import * as PIXI from 'pixi.js';
import type { Sprite, Bird } from '../types/wasm';

/**
 * WasmSpriteView - Bridges WASM Sprite objects with PixiJS rendering
 *
 * This takes a C++ Sprite (or Bird) from WASM and creates a PixiJS
 * visual representation that stays synchronized with the C++ object.
 */
export class WasmSpriteView {
    private wasmSprite: Sprite | Bird;
    private graphics: PIXI.Graphics;
    private destroyed: boolean = false;

    constructor(wasmSprite: Sprite | Bird, stage: PIXI.Container) {
        this.wasmSprite = wasmSprite;
        this.graphics = new PIXI.Graphics();

        // Draw the sprite (we'll enhance this later with actual textures)
        this.drawSprite();

        // Make it interactive
        this.graphics.eventMode = 'static';
        this.graphics.cursor = 'pointer';

        // Add interaction handlers
        this.setupInteractions();

        // Add to stage
        stage.addChild(this.graphics);

        console.log('[WasmSpriteView] Created view for WASM sprite');
    }

    /**
     * Draw a visual representation of the sprite
     * For now, we'll draw different shapes based on type
     */
    private drawSprite(): void {
        const isBird = 'setVelocity' in this.wasmSprite;

        this.graphics.clear();

        if (isBird) {
            // Draw a bird shape
            this.graphics.beginFill(0xFFD700); // Gold
            this.graphics.drawCircle(0, 0, 30); // Body
            this.graphics.endFill();

            this.graphics.beginFill(0xFFFFFF); // White eyes
            this.graphics.drawCircle(-10, -10, 8);
            this.graphics.drawCircle(10, -10, 8);
            this.graphics.endFill();

            this.graphics.beginFill(0x000000); // Black pupils
            this.graphics.drawCircle(-10, -10, 4);
            this.graphics.drawCircle(10, -10, 4);
            this.graphics.endFill();

            this.graphics.beginFill(0xFF6347); // Orange beak
            this.graphics.moveTo(0, 0);
            this.graphics.lineTo(20, -5);
            this.graphics.lineTo(20, 5);
            this.graphics.lineTo(0, 0);
            this.graphics.endFill();

            // Add wings
            this.graphics.beginFill(0xFFA500, 0.7); // Semi-transparent orange
            this.graphics.drawEllipse(-25, 0, 15, 10);
            this.graphics.drawEllipse(25, 0, 15, 10);
            this.graphics.endFill();
        } else {
            // Draw a generic sprite shape
            this.graphics.beginFill(0x4169E1); // Royal blue
            this.graphics.drawRect(-25, -25, 50, 50);
            this.graphics.endFill();

            // Add border
            this.graphics.lineStyle(2, 0xFFFFFF);
            this.graphics.drawRect(-25, -25, 50, 50);
        }

        // Add a label
        const text = new PIXI.Text(isBird ? 'WASM Bird' : 'WASM Sprite', {
            fontSize: 10,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 2
        });
        text.anchor.set(0.5);
        text.y = 45;
        this.graphics.addChild(text);
    }

    /**
     * Set up mouse interactions
     */
    private setupInteractions(): void {
        this.graphics.on('pointerover', () => {
            this.graphics.scale.set(1.2);
        });

        this.graphics.on('pointerout', () => {
            this.graphics.scale.set(1);
        });

        this.graphics.on('pointerdown', () => {
            // Rotate the WASM sprite
            const currentRotation = this.wasmSprite.getRotation();
            this.wasmSprite.setRotation(currentRotation + Math.PI / 4);
            console.log('[WasmSpriteView] Clicked! New rotation:', this.wasmSprite.getRotation());
        });
    }

    /**
     * Update the PixiJS graphics to match the WASM sprite state
     * Call this every frame!
     */
    update(deltaTime: number): void {
        if (this.destroyed) return;

        // Update WASM sprite logic
        this.wasmSprite.update(deltaTime);

        // Sync position from WASM to PixiJS
        this.graphics.x = this.wasmSprite.getX();
        this.graphics.y = this.wasmSprite.getY();

        // Sync rotation
        this.graphics.rotation = this.wasmSprite.getRotation();

        // Sync visibility
        this.graphics.visible = this.wasmSprite.isVisible();
    }

    /**
     * Get the underlying WASM sprite
     */
    getWasmSprite(): Sprite | Bird {
        return this.wasmSprite;
    }

    /**
     * Get the PixiJS graphics object
     */
    getGraphics(): PIXI.Graphics {
        return this.graphics;
    }

    /**
     * Clean up resources
     */
    destroy(): void {
        if (this.destroyed) return;

        this.destroyed = true;
        this.graphics.destroy();
        this.wasmSprite.delete(); // Important: free C++ memory!

        console.log('[WasmSpriteView] Destroyed');
    }
}
