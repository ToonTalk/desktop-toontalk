import { ToonTalkRenderer } from './renderer/renderer';
import { InputManager } from './input/input';
import { getWasmCore } from './core/wasm-core';
import { WasmSpriteView } from './renderer/wasm-sprite-view';

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
        console.log('Creating WASM-powered scene with multiple object types...');

        const wasmCore = getWasmCore();
        if (!wasmCore.isLoaded()) {
            console.error('Cannot create scene - WASM core not loaded');
            return;
        }

        const centerX = 400;
        const centerY = 300;

        // Row 1 (Top): Birds
        const bird1 = wasmCore.createBird(centerX - 150, centerY - 120);
        bird1.setVelocity(0.1, -0.1);
        this.renderer.addWasmSprite(new WasmSpriteView(bird1, this.renderer.getStage(), this.renderer));

        const bird2 = wasmCore.createBird(centerX + 150, centerY - 120);
        bird2.setVelocity(-0.1, -0.05);
        this.renderer.addWasmSprite(new WasmSpriteView(bird2, this.renderer.getStage(), this.renderer));

        // Row 2 (Middle): Numbers
        const num1 = wasmCore.createNumber(centerX - 150, centerY, 42);
        this.renderer.addWasmSprite(new WasmSpriteView(num1, this.renderer.getStage(), this.renderer));

        const num2 = wasmCore.createNumber(centerX + 150, centerY, 3.14);
        this.renderer.addWasmSprite(new WasmSpriteView(num2, this.renderer.getStage(), this.renderer));

        // Row 3 (Bottom-Left): Two Text objects for concatenation testing
        const text1 = wasmCore.createText(centerX - 200, centerY + 120, "Hello");
        this.renderer.addWasmSprite(new WasmSpriteView(text1, this.renderer.getStage(), this.renderer));

        const text2 = wasmCore.createText(centerX - 50, centerY + 120, " World!");
        this.renderer.addWasmSprite(new WasmSpriteView(text2, this.renderer.getStage(), this.renderer));

        // Row 3 (Bottom-Right): Box for storage
        const box1 = wasmCore.createBox(centerX + 150, centerY + 120, 10);
        box1.addItem();
        box1.addItem();
        box1.addItem();
        box1.addItem();
        box1.addItem();  // 5 items in the box
        this.renderer.addWasmSprite(new WasmSpriteView(box1, this.renderer.getStage(), this.renderer));

        // Center: A Nest with 4 holes (2 filled)
        const nest1 = wasmCore.createNest(centerX, centerY, 4);
        nest1.setHole(0, true);  // Fill first hole
        nest1.setHole(2, true);  // Fill third hole
        this.renderer.addWasmSprite(new WasmSpriteView(nest1, this.renderer.getStage(), this.renderer));

        // Row 4 (Bottom-Bottom): Tools - Scale and Wand
        const scale1 = wasmCore.createScale(centerX - 200, centerY + 220);
        this.renderer.addWasmSprite(new WasmSpriteView(scale1, this.renderer.getStage(), this.renderer));

        const wand1 = wasmCore.createWand(centerX - 50, centerY + 220, 1); // Mode 1 = CREATE_TEXT
        this.renderer.addWasmSprite(new WasmSpriteView(wand1, this.renderer.getStage(), this.renderer));

        const wand2 = wasmCore.createWand(centerX + 100, centerY + 220, 4); // Mode 4 = CREATE_BIRD
        this.renderer.addWasmSprite(new WasmSpriteView(wand2, this.renderer.getStage(), this.renderer));

        // Row 5: Advanced - Robot, House, Truck
        const robot1 = wasmCore.createRobot(centerX - 250, centerY + 350);
        robot1.addInstruction();
        robot1.addInstruction();
        robot1.addInstruction();
        robot1.addInstruction();
        robot1.addInstruction(); // 5 instructions
        this.renderer.addWasmSprite(new WasmSpriteView(robot1, this.renderer.getStage(), this.renderer));

        const robot2 = wasmCore.createRobot(centerX - 100, centerY + 350);
        robot2.start(); // Running state
        robot2.addInstruction();
        robot2.addInstruction();
        this.renderer.addWasmSprite(new WasmSpriteView(robot2, this.renderer.getStage(), this.renderer));

        const house1 = wasmCore.createHouse(centerX + 100, centerY + 350, 3);
        house1.setRoomOccupied(0, true);
        house1.setRoomOccupied(1, true); // 2 of 3 rooms occupied
        this.renderer.addWasmSprite(new WasmSpriteView(house1, this.renderer.getStage(), this.renderer));

        const truck1 = wasmCore.createTruck(centerX + 300, centerY + 350);
        truck1.setCargo(true); // Loaded truck
        this.renderer.addWasmSprite(new WasmSpriteView(truck1, this.renderer.getStage(), this.renderer));

        // Row 6: Media & Sensors
        const picture1 = wasmCore.createPicture(centerX - 250, centerY + 500, 120, 100);
        picture1.setImageId(101); // Has an image
        this.renderer.addWasmSprite(new WasmSpriteView(picture1, this.renderer.getStage(), this.renderer));

        const sensor1 = wasmCore.createSensor(centerX - 80, centerY + 500, 0); // MOUSE sensor
        sensor1.setActive(true);
        sensor1.setValue(42.5);
        this.renderer.addWasmSprite(new WasmSpriteView(sensor1, this.renderer.getStage(), this.renderer));

        const sensor2 = wasmCore.createSensor(centerX + 30, centerY + 500, 2); // TIME sensor
        this.renderer.addWasmSprite(new WasmSpriteView(sensor2, this.renderer.getStage(), this.renderer));

        const notebook1 = wasmCore.createNotebook(centerX + 180, centerY + 500, 5);
        notebook1.setPageContent(0, true);
        notebook1.setPageContent(2, true);
        notebook1.setPageContent(3, true); // 3 pages with content
        notebook1.setCurrentPage(2);
        this.renderer.addWasmSprite(new WasmSpriteView(notebook1, this.renderer.getStage(), this.renderer));

        console.log('✨ WASM scene created!');
        console.log('   Objects: 2 Birds, 2 Numbers, 2 Texts, 1 Box, 1 Nest');
        console.log('   Tools: 1 Scale, 2 Wands');
        console.log('   Advanced: 2 Robots, 1 House, 1 Truck');
        console.log('   Media: 1 Picture, 2 Sensors, 1 Notebook');
        console.log('   Total: 18 interactive objects!');
        console.log('');
        console.log('   💡 Drag objects to move them');
        console.log('   💡 Drop Number on Number to add');
        console.log('   💡 Drop Text on Text to concatenate');
        console.log('   💡 Drop anything on Box or Nest to store');
        console.log('   💡 Sensors detect input (Mouse, Keyboard, Time, Collision)');
        console.log('   💡 Picture displays images, Notebook stores notes');
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
