import * as PIXI from 'pixi.js';

/**
 * ToonTalkRenderer - Wrapper around PixiJS
 *
 * Handles all rendering operations for ToonTalk Web.
 * This replaces the DirectDraw/GDI rendering from the C++ version.
 */
export class ToonTalkRenderer {
    private app: PIXI.Application;
    private stage: PIXI.Container;
    private demoSprite?: PIXI.Graphics;

    constructor() {
        // Create PixiJS application with options (v7 uses constructor, not init())
        this.app = new PIXI.Application({
            width: 800,
            height: 600,
            backgroundColor: 0x87CEEB, // Sky blue, similar to ToonTalk background
            resolution: window.devicePixelRatio || 1,
            autoDensity: true,
            antialias: true,
        });
        this.stage = new PIXI.Container();
    }

    async initialize(): Promise<void> {
        // Set up stage
        this.app.stage.addChild(this.stage);

        console.log('PixiJS renderer initialized');
        console.log('Renderer:', this.app.renderer.constructor.name);
    }

    getView(): HTMLCanvasElement {
        return this.app.view as HTMLCanvasElement;
    }

    getRendererInfo(): string {
        // In PixiJS v7, just use the constructor name
        const rendererName = this.app.renderer.constructor.name;
        return `${rendererName} (PixiJS ${PIXI.VERSION})`;
    }

    /**
     * Creates a demo sprite for proof-of-concept
     * This will be replaced with actual ToonTalk sprites from assets
     */
    createDemoSprite(): void {
        // Create a simple animated sprite using graphics
        // TODO: Replace with actual sprite loading from assets

        const graphics = new PIXI.Graphics();

        // Draw a simple "bird" shape (placeholder)
        graphics.beginFill(0xFFD700); // Gold color
        graphics.drawCircle(0, 0, 30); // Body
        graphics.endFill();

        graphics.beginFill(0xFFFFFF);
        graphics.drawCircle(-10, -10, 8); // Left eye
        graphics.drawCircle(10, -10, 8); // Right eye
        graphics.endFill();

        graphics.beginFill(0x000000);
        graphics.drawCircle(-10, -10, 4); // Left pupil
        graphics.drawCircle(10, -10, 4); // Right pupil
        graphics.endFill();

        graphics.beginFill(0xFF6347); // Orange-red beak
        graphics.moveTo(0, 0);
        graphics.lineTo(20, -5);
        graphics.lineTo(20, 5);
        graphics.lineTo(0, 0);
        graphics.endFill();

        // Position in center
        graphics.x = this.app.screen.width / 2;
        graphics.y = this.app.screen.height / 2;

        // Make it interactive
        graphics.eventMode = 'static';
        graphics.cursor = 'pointer';

        // Add hover effect
        graphics.on('pointerover', () => {
            graphics.scale.set(1.2);
        });

        graphics.on('pointerout', () => {
            graphics.scale.set(1);
        });

        graphics.on('pointerdown', () => {
            console.log('Demo sprite clicked!');
            // Simple animation
            graphics.rotation += Math.PI / 4;
        });

        this.stage.addChild(graphics);
        this.demoSprite = graphics;

        console.log('Demo sprite created at center of screen');
    }

    update(deltaTime: number): void {
        // Update animations
        if (this.demoSprite) {
            // Simple floating animation
            const time = Date.now() / 1000;
            this.demoSprite.y = (this.app.screen.height / 2) + Math.sin(time * 2) * 20;
            this.demoSprite.rotation += 0.001 * deltaTime;
        }
    }

    render(): void {
        // PixiJS handles rendering automatically via ticker
        // This method is here for compatibility with the game loop
        // and for future manual render control if needed
    }

    /**
     * Load a sprite from an image file
     * TODO: Implement sprite loading from ToonTalk assets
     */
    async loadSprite(path: string): Promise<PIXI.Sprite> {
        const texture = await PIXI.Assets.load(path);
        return new PIXI.Sprite(texture);
    }

    /**
     * Clear the stage
     */
    clear(): void {
        this.stage.removeChildren();
    }

    /**
     * Resize handler
     */
    resize(width: number, height: number): void {
        this.app.renderer.resize(width, height);
    }

    /**
     * Cleanup
     */
    destroy(): void {
        this.app.destroy(true, { children: true });
    }
}
