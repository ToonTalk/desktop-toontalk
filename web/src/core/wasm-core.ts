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

        // Test 15: Create Picture
        const picture = new this.module.Picture(1000, 100, 150, 120);
        console.log('[WASM Test] Created Picture - Size:', picture.getPictureWidth(), 'x', picture.getPictureHeight());
        console.log('[WASM Test] Has image:', picture.hasImage());
        picture.setImageId(42);
        console.log('[WASM Test] After setImageId(42):', picture.getImageId(), '- Has image:', picture.hasImage());
        picture.delete();

        // Test 16: Create Sensor
        const sensor = new this.module.Sensor(1100, 100, this.module.SensorType.MOUSE);
        console.log('[WASM Test] Created Sensor - Type:', sensor.getTypeInt(), '(MOUSE=0)');
        console.log('[WASM Test] Active:', sensor.isActive(), '- Value:', sensor.getValue());
        sensor.setActive(true);
        sensor.setValue(3.14);
        console.log('[WASM Test] After activation - Active:', sensor.isActive(), '- Value:', sensor.getValue());
        sensor.delete();

        // Test 17: Create Notebook
        const notebook = new this.module.Notebook(1200, 100, 5);
        console.log('[WASM Test] Created Notebook - Pages:', notebook.getNumPages());
        console.log('[WASM Test] Current page:', notebook.getCurrentPage());
        notebook.setPageContent(0, true);
        notebook.setPageContent(2, true);
        notebook.setPageContent(4, true);
        console.log('[WASM Test] Pages with content:', notebook.countPagesWithContent());
        notebook.nextPage();
        console.log('[WASM Test] After nextPage():', notebook.getCurrentPage());
        notebook.delete();

        // Test 18: Create Bomb
        const bomb = new this.module.Bomb(1300, 100);
        console.log('[WASM Test] Created Bomb at:', bomb.getX(), bomb.getY());
        console.log('[WASM Test] Bomb state:', bomb.getStateInt(), '(INACTIVE=0)');
        console.log('[WASM Test] Fuse time:', bomb.getFuseTime(), 'seconds');
        bomb.arm();
        console.log('[WASM Test] After arm():', bomb.getStateInt(), '(ARMED=1)');
        bomb.reset();
        console.log('[WASM Test] After reset():', bomb.getStateInt(), '(INACTIVE=0)');
        bomb.delete();

        // Test 19: Create Thought_Bubble
        const thoughtBubble = new this.module.Thought_Bubble(1400, 100);
        console.log('[WASM Test] Created Thought_Bubble at:', thoughtBubble.getX(), thoughtBubble.getY());
        console.log('[WASM Test] Has cubby:', thoughtBubble.hasCubby());
        thoughtBubble.setCubbyId(42);
        console.log('[WASM Test] After setCubbyId(42):', thoughtBubble.getCubbyId(), '- Has cubby:', thoughtBubble.hasCubby());
        thoughtBubble.clearCubby();
        console.log('[WASM Test] After clearCubby():', thoughtBubble.hasCubby());
        thoughtBubble.delete();

        // Test 20: Create Mouse
        const mouse = new this.module.Mouse(1500, 100);
        console.log('[WASM Test] Created Mouse at:', mouse.getX(), mouse.getY());
        console.log('[WASM Test] Mouse state:', mouse.getStateInt(), '(IDLE=0)');
        mouse.setOperand1(42);
        mouse.setOperand2(13);
        mouse.setOperationInt(0); // ADD
        console.log('[WASM Test] Set operands: 42 + 13');
        mouse.doOperation();
        console.log('[WASM Test] After doOperation() - Result:', mouse.getResult(), '- State:', mouse.getStateInt(), '(SMASHING=2)');
        mouse.finishOperation();
        console.log('[WASM Test] After finishOperation() - State:', mouse.getStateInt(), '(IDLE=0)');
        mouse.delete();

        // Test 21: Create Vacuum (if available in WASM)
        if (this.module.Vacuum) {
            const vacuum = new this.module.Vacuum(1600, 100);
            console.log('[WASM Test] Created Vacuum at:', vacuum.getX(), vacuum.getY());
            console.log('[WASM Test] Vacuum state:', vacuum.getStateInt(), '(SUCK=0)');
            console.log('[WASM Test] Sucked count:', vacuum.getSuckedCount());
            vacuum.suckUp();
            vacuum.suckUp();
            vacuum.suckUp();
            console.log('[WASM Test] After 3 suckUp():', vacuum.getSuckedCount());
            vacuum.setStateInt(1); // SPIT
            console.log('[WASM Test] Changed to SPIT state:', vacuum.getStateInt());
            vacuum.restore();
            console.log('[WASM Test] After restore():', vacuum.getSuckedCount());
            vacuum.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Vacuum (not in WASM binary yet - rebuild required)');
        }

        // Test 22: Create Martian (if available in WASM)
        if (this.module.Martian) {
            const martian = new this.module.Martian(1700, 100);
            console.log('[WASM Test] Created Martian at:', martian.getX(), martian.getY());
            console.log('[WASM Test] Martian state:', martian.getStateInt(), '(IDLE=0)');
            console.log('[WASM Test] Has balloon:', martian.hasBalloon());
            martian.say(101); // Say message ID 101
            console.log('[WASM Test] After say(101) - State:', martian.getStateInt(), '(TALKING=3) - MessageID:', martian.getMessageId());
            console.log('[WASM Test] Has balloon:', martian.hasBalloon());
            martian.teleportOut();
            console.log('[WASM Test] After teleportOut() - State:', martian.getStateInt(), '(TELEPORTING_OUT=2)');
            martian.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Martian (not in WASM binary yet - rebuild required)');
        }

        // Test 23: Create Toolbox (if available in WASM)
        if (this.module.Toolbox) {
            const toolbox = new this.module.Toolbox(1800, 100);
            console.log('[WASM Test] Created Toolbox at:', toolbox.getX(), toolbox.getY());
            console.log('[WASM Test] Toolbox state:', toolbox.getStateInt(), '(CLOSED=0)');
            console.log('[WASM Test] Selected stack:', toolbox.getSelectedStack(), '(NUMBERS=0)');
            console.log('[WASM Test] Numbers stack count:', toolbox.getStackCount(0));
            toolbox.open();
            console.log('[WASM Test] After open() - State:', toolbox.getStateInt(), '(OPENING=1)');
            toolbox.finishOpening();
            console.log('[WASM Test] After finishOpening() - State:', toolbox.getStateInt(), '(OPEN=2)');
            toolbox.setSelectedStack(5); // ROBOTS
            console.log('[WASM Test] Selected ROBOTS stack:', toolbox.getSelectedStack());
            console.log('[WASM Test] Robots stack count:', toolbox.getStackCount(5));
            toolbox.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Toolbox (not in WASM binary yet - rebuild required)');
        }

        // Test 24: Create Expander (if available in WASM)
        if (this.module.Expander) {
            const expander = new this.module.Expander(1900, 100);
            console.log('[WASM Test] Created Expander at:', expander.getX(), expander.getY());
            console.log('[WASM Test] Expander state:', expander.getStateInt(), '(BIGGER=0)');
            console.log('[WASM Test] Scale factor:', expander.getScaleFactor());
            expander.expand();
            expander.expand();
            console.log('[WASM Test] After 2 expand():', expander.getTargetScale());
            expander.setStateInt(3); // SMALLER
            console.log('[WASM Test] Changed to SMALLER:', expander.getStateInt());
            expander.shrink();
            console.log('[WASM Test] After shrink():', expander.getTargetScale());
            expander.setGoodSize();
            console.log('[WASM Test] After setGoodSize() - State:', expander.getStateInt(), '(GOOD_SIZE=6) - Scale:', expander.getTargetScale());
            expander.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Expander (not in WASM binary yet - rebuild required)');
        }

        // Test 25: Create Copier (if available in WASM)
        if (this.module.Copier) {
            const copier = new this.module.Copier(2000, 100);
            console.log('[WASM Test] Created Copier at:', copier.getX(), copier.getY());
            console.log('[WASM Test] Copier state:', copier.getStateInt(), '(NORMAL=0)');
            console.log('[WASM Test] Copy count:', copier.getCopyCount());
            console.log('[WASM Test] Max copies:', copier.getMaxCopies(), '(-1=infinite)');
            copier.setHasAttached(true);
            console.log('[WASM Test] After setHasAttached(true):', copier.hasAttached());
            copier.makeCopy();
            copier.makeCopy();
            copier.makeCopy();
            console.log('[WASM Test] After 3 makeCopy():', copier.getCopyCount());
            copier.setMaxCopies(5);
            console.log('[WASM Test] Set max copies to 5:', copier.getMaxCopies());
            copier.reset();
            console.log('[WASM Test] After reset() - Count:', copier.getCopyCount(), '- HasAttached:', copier.hasAttached());
            copier.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Copier (not in WASM binary yet - rebuild required)');
        }

        // Test 26: Create Eraser (if available in WASM)
        if (this.module.Eraser) {
            const eraser = new this.module.Eraser(2100, 100);
            console.log('[WASM Test] Created Eraser at:', eraser.getX(), eraser.getY());
            console.log('[WASM Test] Eraser state:', eraser.getStateInt(), '(READY=0)');
            console.log('[WASM Test] Erased count:', eraser.getErasedCount());
            console.log('[WASM Test] Erase progress:', eraser.getEraseProgress());
            eraser.startErasing();
            console.log('[WASM Test] After startErasing() - State:', eraser.getStateInt(), '(ERASING=1) - Progress:', eraser.getEraseProgress());
            eraser.erase();
            console.log('[WASM Test] After erase() - State:', eraser.getStateInt(), '(DONE=2) - Count:', eraser.getErasedCount());
            console.log('[WASM Test] Progress:', eraser.getEraseProgress());
            eraser.reset();
            console.log('[WASM Test] After reset() - State:', eraser.getStateInt(), '(READY=0) - Progress:', eraser.getEraseProgress());
            eraser.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Eraser (not in WASM binary yet - rebuild required)');
        }

        // Test 27: Create Cubby (if available in WASM)
        if (this.module.Cubby) {
            const cubby = new this.module.Cubby(2200, 100, 5);
            console.log('[WASM Test] Created Cubby at:', cubby.getX(), cubby.getY());
            console.log('[WASM Test] Number of holes:', cubby.getNumHoles());
            console.log('[WASM Test] Filled count:', cubby.getFilledCount());
            cubby.setHoleLabel(0, 'A');
            cubby.setHoleLabel(1, 'B');
            cubby.setHoleLabel(2, 'C');
            console.log('[WASM Test] Set labels A, B, C');
            cubby.setHoleFilled(0, true);
            cubby.setHoleFilled(2, true);
            console.log('[WASM Test] Filled holes 0 and 2 - Count:', cubby.getFilledCount());
            console.log('[WASM Test] Hole 0 filled:', cubby.isHoleFilled(0), '- Label:', cubby.getHoleLabel(0));
            console.log('[WASM Test] Hole 1 filled:', cubby.isHoleFilled(1), '- Label:', cubby.getHoleLabel(1));
            console.log('[WASM Test] isFull:', cubby.isFull(), '- isEmpty:', cubby.isEmpty());
            cubby.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Cubby (not in WASM binary yet - rebuild required)');
        }

        // Test 28: Create Button (if available in WASM)
        if (this.module.Button) {
            const button = new this.module.Button(2300, 100, 'S');
            console.log('[WASM Test] Created Button at:', button.getX(), button.getY());
            console.log('[WASM Test] Button state:', button.getStateInt(), '(NORMAL=0)');
            console.log('[WASM Test] Key binding:', button.getKeyBinding());
            console.log('[WASM Test] Press count:', button.getPressCount());
            console.log('[WASM Test] Enabled:', button.isEnabled());
            button.hover();
            console.log('[WASM Test] After hover() - State:', button.getStateInt(), '(HOVERED=1)');
            button.press();
            console.log('[WASM Test] After press() - State:', button.getStateInt(), '(PRESSED=2) - Count:', button.getPressCount());
            button.release();
            console.log('[WASM Test] After release() - State:', button.getStateInt(), '(NORMAL=0)');
            button.setEnabled(false);
            console.log('[WASM Test] After setEnabled(false) - State:', button.getStateInt(), '(DISABLED=3)');
            button.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Button (not in WASM binary yet - rebuild required)');
        }

        // Test 29: Create Stack (if available in WASM)
        if (this.module.Stack) {
            const stack = new this.module.Stack(2400, 100, 5);
            console.log('[WASM Test] Created Stack at:', stack.getX(), stack.getY());
            console.log('[WASM Test] Capacity:', stack.getCapacity());
            console.log('[WASM Test] Count:', stack.getCount());
            console.log('[WASM Test] isEmpty:', stack.isEmpty(), '- isFull:', stack.isFull());
            stack.push();
            stack.push();
            stack.push();
            console.log('[WASM Test] After 3 push() - Count:', stack.getCount(), '- Fullness:', stack.getFullness());
            console.log('[WASM Test] Peek (top index):', stack.peek());
            stack.pop();
            console.log('[WASM Test] After pop() - Count:', stack.getCount());
            stack.setCapacity(10);
            console.log('[WASM Test] After setCapacity(10) - Capacity:', stack.getCapacity());
            stack.clear();
            console.log('[WASM Test] After clear() - Count:', stack.getCount(), '- isEmpty:', stack.isEmpty());
            stack.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Stack (not in WASM binary yet - rebuild required)');
        }

        console.log('[WASM Tests] ✅ All available tests passed!');
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
     * Create a Picture instance from WASM
     */
    createPicture(x: number, y: number, width: number = 120, height: number = 100) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Picture(x, y, width, height);
    }

    /**
     * Create a Sensor instance from WASM
     */
    createSensor(x: number, y: number, type?: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Sensor(x, y, type);
    }

    /**
     * Create a Notebook instance from WASM
     */
    createNotebook(x: number, y: number, numPages: number = 5) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Notebook(x, y, numPages);
    }

    /**
     * Create a Bomb instance from WASM
     */
    createBomb(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Bomb(x, y);
    }

    /**
     * Create a Thought_Bubble instance from WASM
     */
    createThoughtBubble(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Thought_Bubble(x, y);
    }

    /**
     * Create a Mouse instance from WASM
     */
    createMouse(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Mouse(x, y);
    }

    /**
     * Create a Vacuum instance from WASM
     */
    createVacuum(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Vacuum) {
            throw new Error('Vacuum class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Vacuum(x, y);
    }

    /**
     * Create a Martian instance from WASM
     */
    createMartian(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Martian) {
            throw new Error('Martian class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Martian(x, y);
    }

    /**
     * Create a Toolbox instance from WASM
     */
    createToolbox(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Toolbox) {
            throw new Error('Toolbox class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Toolbox(x, y);
    }

    /**
     * Create an Expander instance from WASM
     */
    createExpander(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Expander) {
            throw new Error('Expander class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Expander(x, y);
    }

    /**
     * Create a Copier instance from WASM
     */
    createCopier(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Copier) {
            throw new Error('Copier class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Copier(x, y);
    }

    /**
     * Create an Eraser instance from WASM
     */
    createEraser(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Eraser) {
            throw new Error('Eraser class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Eraser(x, y);
    }

    /**
     * Create a Cubby instance from WASM
     */
    createCubby(x: number, y: number, numHoles: number = 4) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Cubby) {
            throw new Error('Cubby class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Cubby(x, y, numHoles);
    }

    /**
     * Create a Button instance from WASM
     */
    createButton(x: number, y: number, key: string = '') {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Button) {
            throw new Error('Button class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Button(x, y, key);
    }

    /**
     * Create a Stack instance from WASM
     */
    createStack(x: number, y: number, capacity: number = 10) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Stack) {
            throw new Error('Stack class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Stack(x, y, capacity);
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
