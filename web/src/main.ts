import { ToonTalkRenderer } from './renderer/renderer';
import { InputManager } from './input/input';
import { getWasmCore } from './core/wasm-core';

/**
 * ToonTalk Web - Main Entry Point
 *
 * This proof-of-concept demonstrates:
 * 1. PixiJS rendering initialization
 * 2. Basic sprite display
 * 3. Mouse interaction
 * 4. WASM module integration (C++ core)
 */

class ToonTalkWeb {
    private renderer: ToonTalkRenderer;
    private inputManager: InputManager;
    private lastTime: number = 0;
    private fps: number = 0;

    constructor() {
        this.renderer = new ToonTalkRenderer();
        this.inputManager = new InputManager();

        this.updateStatus('Initializing renderer...');
    }

    async initialize(): Promise<void> {
        try {
            // Initialize renderer
            await this.renderer.initialize();
            this.updateStatus('Renderer initialized');

            // Mount to DOM
            const container = document.getElementById('game-container');
            if (!container) {
                throw new Error('Game container not found');
            }

            container.appendChild(this.renderer.getView());
            this.updateStatus('Canvas mounted');

            // Initialize input
            this.inputManager.initialize(this.renderer.getView());
            this.updateStatus('Input system ready');

            // Load WASM module
            this.updateStatus('Loading WASM core...');
            const wasmCore = getWasmCore();
            await wasmCore.initialize();
            this.updateStatus('WASM core loaded');

            // Create demo content
            this.createDemoScene();

            // Hide loading screen
            const loading = document.getElementById('loading');
            if (loading) {
                loading.classList.add('hidden');
            }

            // Update renderer info
            this.updateRendererInfo();

            // Start game loop
            this.updateStatus('Running');
            this.startGameLoop();

        } catch (error) {
            console.error('Initialization failed:', error);
            this.updateStatus(`Error: ${error}`);
        }
    }

    private createDemoScene(): void {
        // Create a simple demo scene
        // This will be replaced with actual ToonTalk objects from WASM
        this.renderer.createDemoSprite();

        console.log('Demo scene created');
    }

    private startGameLoop(): void {
        const gameLoop = (currentTime: number) => {
            // Calculate delta time
            const deltaTime = currentTime - this.lastTime;
            this.lastTime = currentTime;

            // Update FPS counter (every second)
            if (deltaTime > 0) {
                this.fps = Math.round(1000 / deltaTime);
                this.updateFPS();
            }

            // Update game state
            this.update(deltaTime);

            // Render
            this.renderer.render();

            // Continue loop
            requestAnimationFrame(gameLoop);
        };

        requestAnimationFrame(gameLoop);
    }

    private update(deltaTime: number): void {
        // Update game logic
        // TODO: Call WASM update functions
        this.renderer.update(deltaTime);
    }

    private updateStatus(status: string): void {
        const statusElement = document.getElementById('status');
        if (statusElement) {
            statusElement.textContent = status;
        }
        console.log(`[Status] ${status}`);
    }

    private updateFPS(): void {
        const fpsElement = document.getElementById('fps');
        if (fpsElement) {
            fpsElement.textContent = `FPS: ${this.fps}`;
        }
    }

    private updateRendererInfo(): void {
        const info = this.renderer.getRendererInfo();
        const infoElement = document.getElementById('renderer-info');
        if (infoElement) {
            infoElement.textContent = `Renderer: ${info}`;
        }
    }
}

// Initialize when DOM is ready
document.addEventListener('DOMContentLoaded', async () => {
    console.log('ToonTalk Web starting...');

    const app = new ToonTalkWeb();
    await app.initialize();

    console.log('ToonTalk Web initialized successfully!');
});
