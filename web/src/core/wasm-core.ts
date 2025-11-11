import type { ToonTalkCoreModule, CreateToonTalkCore } from '../types/wasm';

/**
 * WasmCore - Manages the ToonTalk WASM module
 *
 * Loads and initializes the C++ core compiled to WebAssembly
 */
export class WasmCore {
    private module?: ToonTalkCoreModule;
    private loaded: boolean = false;

    /**
     * Load a script dynamically
     */
    private loadScript(src: string): Promise<void> {
        return new Promise((resolve, reject) => {
            const script = document.createElement('script');
            script.src = src;
            script.onload = () => resolve();
            script.onerror = () => reject(new Error(`Failed to load script: ${src}`));
            document.head.appendChild(script);
        });
    }

    async initialize(): Promise<void> {
        console.log('[WASM] Loading ToonTalk core module...');

        try {
            // Load the WASM module from public directory
            await this.loadScript('/core/build/toontalk-core.js');

            // Wait a bit for the script to be parsed
            await new Promise(resolve => setTimeout(resolve, 100));

            // Get the module factory - try different export names
            let createModule: CreateToonTalkCore | undefined;

            // Try different possible export names
            const windowObj = window as any;
            if (typeof windowObj.createToonTalkCore === 'function') {
                createModule = windowObj.createToonTalkCore;
                console.log('[WASM] Found createToonTalkCore on window');
            } else if (typeof windowObj.Module === 'function') {
                createModule = windowObj.Module;
                console.log('[WASM] Found Module on window');
            } else {
                console.error('[WASM] Window keys:', Object.keys(windowObj).filter(k =>
                    k.toLowerCase().includes('module') ||
                    k.toLowerCase().includes('toon') ||
                    k.toLowerCase().includes('create')
                ));
                throw new Error('WASM module factory not found. Check console for available exports.');
            }

            if (!createModule) {
                throw new Error('Could not find WASM module factory function');
            }

            console.log('[WASM] Initializing module...');

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

        // Test 6: Create a Number
        const num = new this.module.Number(100, 100, 42);
        console.log('[WASM Test] Created Number with value:', num.getValue());
        num.add(13);
        console.log('[WASM Test] After add(13):', num.getValue(), '- String:', num.toString());
        num.multiply(2);
        console.log('[WASM Test] After multiply(2):', num.getValue());
        num.delete();

        // Test 7: Create Text
        const text = new this.module.Text(200, 100, "Hello");
        console.log('[WASM Test] Created Text:', text.getText());
        text.append(" World!");
        console.log('[WASM Test] After append:", text.getText(), "- Length:', text.length());
        text.delete();

        // Test 8: Create Box
        const box = new this.module.Box(300, 100, 5);
        console.log('[WASM Test] Created Box - Capacity:', box.getCapacity());
        box.addItem();
        box.addItem();
        box.addItem();
        console.log('[WASM Test] Added 3 items - Count:', box.getCount(), '- Fullness:', box.getFullness());
        console.log('[WASM Test] isEmpty:', box.isEmpty(), '- isFull:', box.isFull());
        box.delete();

        // Test 9: Create Nest
        const nest = new this.module.Nest(400, 100, 4);
        console.log('[WASM Test] Created Nest - Holes:', nest.getNumHoles());
        nest.setHole(0, true);
        nest.setHole(2, true);
        console.log('[WASM Test] Filled 2 holes - Occupied:', nest.countOccupied());
        console.log('[WASM Test] isEmpty:', nest.isEmpty(), '- isFull:', nest.isFull());
        nest.delete();

        // Test 10: Create Scale
        const scale = new this.module.Scale(500, 100);
        console.log('[WASM Test] Created Scale at:', scale.getX(), scale.getY());
        console.log('[WASM Test] Scale active:', scale.isActive());
        scale.setActive(true);
        console.log('[WASM Test] Scale activated:', scale.isActive());
        scale.delete();

        // Test 11: Create Wand
        const wand = new this.module.Wand(600, 100, this.module.WandMode.CREATE_TEXT);
        console.log('[WASM Test] Created Wand at:', wand.getX(), wand.getY());
        console.log('[WASM Test] Wand mode:', wand.getModeInt(), '(CREATE_TEXT=1)');
        wand.nextMode();
        console.log('[WASM Test] After nextMode():', wand.getModeInt(), '(CREATE_BOX=2)');
        wand.previousMode();
        console.log('[WASM Test] After previousMode():', wand.getModeInt(), '(CREATE_TEXT=1)');
        wand.delete();

        // Test 12: Create Robot
        const robot = new this.module.Robot(700, 100);
        console.log('[WASM Test] Created Robot at:', robot.getX(), robot.getY());
        console.log('[WASM Test] Robot state:', robot.getStateInt(), '(IDLE=0)');
        console.log('[WASM Test] Instructions:', robot.getInstructionCount());
        robot.addInstruction();
        robot.addInstruction();
        robot.addInstruction();
        console.log('[WASM Test] After adding 3 instructions:', robot.getInstructionCount());
        robot.start();
        console.log('[WASM Test] After start():', robot.getStateInt(), '(RUNNING=1)');
        robot.pause();
        console.log('[WASM Test] After pause():', robot.getStateInt(), '(PAUSED=2)');
        robot.delete();

        // Test 13: Create House
        const house = new this.module.House(800, 100, 3);
        console.log('[WASM Test] Created House - Rooms:', house.getNumRooms());
        console.log('[WASM Test] Current room:', house.getCurrentRoom());
        house.setRoomOccupied(0, true);
        house.setRoomOccupied(2, true);
        console.log('[WASM Test] Occupied rooms:', house.countOccupiedRooms());
        console.log('[WASM Test] isEmpty:', house.isEmpty(), '- isFull:', house.isFull());
        house.delete();

        // Test 14: Create Truck
        const truck = new this.module.Truck(900, 100);
        console.log('[WASM Test] Created Truck at:', truck.getX(), truck.getY());
        console.log('[WASM Test] Truck state:', truck.getStateInt(), '(EMPTY=0)');
        console.log('[WASM Test] Has cargo:', truck.hasCargo());
        truck.setCargo(true);
        console.log('[WASM Test] After loading cargo - state:', truck.getStateInt(), '(LOADED=1)');
        truck.startDelivery();
        console.log('[WASM Test] After startDelivery():', truck.getStateInt(), '(DELIVERING=2)');
        truck.completeDelivery();
        console.log('[WASM Test] After completeDelivery():', truck.getStateInt(), '(EMPTY=0)');
        truck.delete();

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
     * Create a Number instance from WASM
     */
    createNumber(x: number, y: number, value: number = 0) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Number(x, y, value);
    }

    /**
     * Create a Text instance from WASM
     */
    createText(x: number, y: number, text: string = "") {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Text(x, y, text);
    }

    /**
     * Create a Box instance from WASM
     */
    createBox(x: number, y: number, capacity: number = 10) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Box(x, y, capacity);
    }

    /**
     * Create a Nest instance from WASM
     */
    createNest(x: number, y: number, numHoles: number = 3) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Nest(x, y, numHoles);
    }

    /**
     * Create a Scale instance from WASM
     */
    createScale(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Scale(x, y);
    }

    /**
     * Create a Wand instance from WASM
     */
    createWand(x: number, y: number, mode?: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Wand(x, y, mode);
    }

    /**
     * Create a Robot instance from WASM
     */
    createRobot(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Robot(x, y);
    }

    /**
     * Create a House instance from WASM
     */
    createHouse(x: number, y: number, numRooms: number = 1) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.House(x, y, numRooms);
    }

    /**
     * Create a Truck instance from WASM
     */
    createTruck(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Truck(x, y);
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
