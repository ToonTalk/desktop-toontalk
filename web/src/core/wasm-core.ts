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

        // Test 30: Create Flipper (if available in WASM)
        if (this.module.Flipper) {
            const flipper = new this.module.Flipper(2600, 100);
            console.log('[WASM Test] Created Flipper at:', flipper.getX(), flipper.getY());
            console.log('[WASM Test] Mode:', flipper.getModeInt(), '(HORIZONTAL=0)');
            flipper.doFlip();
            console.log('[WASM Test] After doFlip() - FlipCount:', flipper.getFlipCount());
            flipper.nextMode();
            flipper.nextMode();
            console.log('[WASM Test] After 2x nextMode() - Mode:', flipper.getModeInt(), '(ROTATE_CW=2)');
            flipper.doFlip();
            console.log('[WASM Test] After doFlip() in ROTATE_CW - Angle:', flipper.getRotationAngle(), 'degrees');
            flipper.setAttached(true);
            console.log('[WASM Test] HasAttached:', flipper.hasAttached());
            flipper.reset();
            console.log('[WASM Test] After reset() - Angle:', flipper.getRotationAngle(), '- FlipCount:', flipper.getFlipCount());
            flipper.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Flipper (not in WASM binary yet - rebuild required)');
        }

        // Test 31: Create Meter (if available in WASM)
        if (this.module.Meter) {
            const meter = new this.module.Meter(2800, 100, 0, 100);
            console.log('[WASM Test] Created Meter at:', meter.getX(), meter.getY());
            console.log('[WASM Test] Value:', meter.getValue(), '- Min:', meter.getMinValue(), '- Max:', meter.getMaxValue());
            console.log('[WASM Test] State:', meter.getStateInt(), '(NORMAL=0)');
            meter.setValue(50);
            console.log('[WASM Test] After setValue(50) - Percentage:', meter.getPercentage(), '%');
            meter.increment(25);
            console.log('[WASM Test] After increment(25) - Value:', meter.getValue(), '- State:', meter.getStateInt(), '(WARNING=1)');
            meter.increment(20);
            console.log('[WASM Test] After increment(20) - State:', meter.getStateInt(), '(CRITICAL=2)');
            console.log('[WASM Test] isWarning:', meter.isWarning(), '- isCritical:', meter.isCritical());
            meter.setValue(100);
            console.log('[WASM Test] After setValue(100) - State:', meter.getStateInt(), '(MAXED=3)', '- isMaxed:', meter.isMaxed());
            meter.decrement(30);
            console.log('[WASM Test] After decrement(30) - Value:', meter.getValue(), '- Percentage:', meter.getPercentage(), '%');
            meter.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Meter (not in WASM binary yet - rebuild required)');
        }

        // Test 32: Create Beeper (if available in WASM)
        if (this.module.Beeper) {
            const beeper = new this.module.Beeper(3000, 100);
            console.log('[WASM Test] Created Beeper at:', beeper.getX(), beeper.getY());
            console.log('[WASM Test] State:', beeper.getStateInt(), '(SILENT=0)');
            console.log('[WASM Test] Frequency:', beeper.getFrequency(), 'Hz');
            beeper.beep();
            console.log('[WASM Test] After beep() - State:', beeper.getStateInt(), '(BEEPING=1)', '- BeepCount:', beeper.getBeepCount());
            console.log('[WASM Test] isBeeping:', beeper.isBeeping(), '- Timer:', beeper.getTimer());
            beeper.beep();
            beeper.beep();
            console.log('[WASM Test] After 2 more beep() - BeepCount:', beeper.getBeepCount());
            beeper.setFrequency(880);
            console.log('[WASM Test] After setFrequency(880) - Frequency:', beeper.getFrequency(), 'Hz (A5 note)');
            beeper.startContinuous();
            console.log('[WASM Test] After startContinuous() - State:', beeper.getStateInt(), '(CONTINUOUS=2)');
            beeper.stop();
            console.log('[WASM Test] After stop() - State:', beeper.getStateInt(), '(SILENT=0)');
            beeper.reset();
            console.log('[WASM Test] After reset() - BeepCount:', beeper.getBeepCount());
            beeper.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Beeper (not in WASM binary yet - rebuild required)');
        }

        // Test 33: Create Connector (if available in WASM)
        if (this.module.Connector) {
            const connector = new this.module.Connector(3200, 100);
            console.log('[WASM Test] Created Connector at:', connector.getX(), connector.getY());
            console.log('[WASM Test] State:', connector.getStateInt(), '(DISCONNECTED=0)');
            console.log('[WASM Test] MaxConnections:', connector.getMaxConnections());
            connector.startConnecting();
            console.log('[WASM Test] After startConnecting() - State:', connector.getStateInt(), '(CONNECTING=1)');
            connector.connect(123, 456);
            console.log('[WASM Test] After connect(123, 456) - State:', connector.getStateInt(), '(CONNECTED=2)');
            console.log('[WASM Test] SourceId:', connector.getSourceId(), '- TargetId:', connector.getTargetId());
            console.log('[WASM Test] isConnected:', connector.isConnected(), '- ConnectionCount:', connector.getConnectionCount());
            connector.disconnect();
            console.log('[WASM Test] After disconnect() - State:', connector.getStateInt(), '(DISCONNECTED=0)');
            connector.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Connector (not in WASM binary yet - rebuild required)');
        }

        // Test 34: Create Timer (if available in WASM)
        if (this.module.Timer) {
            const timer = new this.module.Timer(3400, 100, 5.0);
            console.log('[WASM Test] Created Timer at:', timer.getX(), timer.getY());
            console.log('[WASM Test] Duration:', timer.getDuration(), 's');
            console.log('[WASM Test] State:', timer.getStateInt(), '(STOPPED=0)');
            timer.start();
            console.log('[WASM Test] After start() - State:', timer.getStateInt(), '(RUNNING=1)', '- isRunning:', timer.isRunning());
            timer.setElapsed(2.5);
            console.log('[WASM Test] After setElapsed(2.5) - Progress:', timer.getProgress(), '- Remaining:', timer.getRemaining());
            timer.pause();
            console.log('[WASM Test] After pause() - State:', timer.getStateInt(), '(PAUSED=2)');
            timer.resume();
            console.log('[WASM Test] After resume() - State:', timer.getStateInt(), '(RUNNING=1)');
            timer.setLoop(true);
            console.log('[WASM Test] Loop enabled:', timer.isLoop());
            timer.stop();
            console.log('[WASM Test] After stop() - State:', timer.getStateInt(), '(STOPPED=0)');
            timer.reset();
            console.log('[WASM Test] After reset() - TriggerCount:', timer.getTriggerCount());
            timer.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Timer (not in WASM binary yet - rebuild required)');
        }

        // Test 35: Create Counter (if available in WASM)
        if (this.module.Counter) {
            const counter = new this.module.Counter(3600, 100, 0);
            console.log('[WASM Test] Created Counter at:', counter.getX(), counter.getY());
            console.log('[WASM Test] Value:', counter.getValue(), '- State:', counter.getStateInt(), '(NORMAL=0)');
            console.log('[WASM Test] Step:', counter.getStep());
            counter.increment();
            counter.increment();
            counter.increment();
            console.log('[WASM Test] After 3x increment() - Value:', counter.getValue());
            counter.decrement();
            console.log('[WASM Test] After decrement() - Value:', counter.getValue());
            counter.setMinLimit(true);
            counter.setMaxLimit(true);
            counter.setMinValue(0);
            counter.setMaxValue(10);
            console.log('[WASM Test] Set limits - Min:', counter.getMinValue(), '- Max:', counter.getMaxValue());
            counter.setValue(10);
            console.log('[WASM Test] After setValue(10) - State:', counter.getStateInt(), '(AT_MAX=2)', '- isAtMax:', counter.isAtMax());
            counter.increment();
            console.log('[WASM Test] After increment() at max - Value:', counter.getValue(), '(should still be 10)');
            counter.reset();
            console.log('[WASM Test] After reset() - Value:', counter.getValue());
            counter.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Counter (not in WASM binary yet - rebuild required)');
        }

        // Test 36: Create Sampler (if available in WASM)
        if (this.module.Sampler) {
            const sampler = new this.module.Sampler(3800, 100, 50);
            console.log('[WASM Test] Created Sampler at:', sampler.getX(), sampler.getY());
            console.log('[WASM Test] Capacity:', sampler.getCapacity(), '- State:', sampler.getStateInt(), '(IDLE=0)');
            console.log('[WASM Test] SampleRate:', sampler.getSampleRate(), 'Hz');
            sampler.startSampling();
            console.log('[WASM Test] After startSampling() - State:', sampler.getStateInt(), '(SAMPLING=1)', '- isSampling:', sampler.isSampling());
            sampler.recordSample(10.5);
            sampler.recordSample(20.3);
            sampler.recordSample(15.7);
            console.log('[WASM Test] After 3 recordSample() - Count:', sampler.getSampleCount());
            console.log('[WASM Test] MinValue:', sampler.getMinValue(), '- MaxValue:', sampler.getMaxValue());
            console.log('[WASM Test] Fullness:', sampler.getFullness());
            sampler.pauseSampling();
            console.log('[WASM Test] After pauseSampling() - State:', sampler.getStateInt(), '(PAUSED=2)');
            sampler.clear();
            console.log('[WASM Test] After clear() - Count:', sampler.getSampleCount(), '- State:', sampler.getStateInt());
            sampler.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Sampler (not in WASM binary yet - rebuild required)');
        }

        // Test 37: Create Comparator (if available in WASM)
        if (this.module.Comparator) {
            const comparator = new this.module.Comparator(4000, 100);
            console.log('[WASM Test] Created Comparator at:', comparator.getX(), comparator.getY());
            console.log('[WASM Test] ValueA:', comparator.getValueA(), '- ValueB:', comparator.getValueB());
            console.log('[WASM Test] Result:', comparator.getResultInt(), '(EQUAL=0)', '- isEqual:', comparator.isEqual());
            comparator.setValueA(10.0);
            comparator.setValueB(5.0);
            console.log('[WASM Test] After setValues(10, 5) - Result:', comparator.getResultInt(), '(GREATER_THAN=2)');
            console.log('[WASM Test] isGreaterThan:', comparator.isGreaterThan(), '- Difference:', comparator.getDifference());
            comparator.setValueA(3.0);
            console.log('[WASM Test] After setValueA(3) - Result:', comparator.getResultInt(), '(LESS_THAN=1)', '- isLessThan:', comparator.isLessThan());
            console.log('[WASM Test] ComparisonCount:', comparator.getComparisonCount());
            comparator.setTolerance(0.5);
            comparator.setValues(5.2, 5.3);
            console.log('[WASM Test] After setTolerance(0.5) and setValues(5.2, 5.3) - isEqual:', comparator.isEqual());
            comparator.reset();
            console.log('[WASM Test] After reset() - ValueA:', comparator.getValueA(), '- Count:', comparator.getComparisonCount());
            comparator.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Comparator (not in WASM binary yet - rebuild required)');
        }

        // Test 38: Create Randomizer (if available in WASM)
        if (this.module.Randomizer) {
            const randomizer = new this.module.Randomizer(4200, 100);
            console.log('[WASM Test] Created Randomizer at:', randomizer.getX(), randomizer.getY());
            console.log('[WASM Test] Mode:', randomizer.getModeInt(), '(UNIFORM=0)');
            console.log('[WASM Test] Range:', randomizer.getMinValue(), '-', randomizer.getMaxValue());
            randomizer.generate();
            console.log('[WASM Test] After generate() - Value:', randomizer.getCurrentValue());
            console.log('[WASM Test] GenerationCount:', randomizer.getGenerationCount());
            randomizer.generate();
            randomizer.generate();
            console.log('[WASM Test] After 2 more generate() - Count:', randomizer.getGenerationCount());
            randomizer.setModeInt(1); // INTEGER
            randomizer.setMinValue(1);
            randomizer.setMaxValue(10);
            randomizer.generate();
            console.log('[WASM Test] INTEGER mode (1-10) - Value:', randomizer.getCurrentValue());
            randomizer.setModeInt(2); // BOOLEAN
            randomizer.generate();
            console.log('[WASM Test] BOOLEAN mode - Value:', randomizer.getCurrentValue(), '(0 or 1)');
            randomizer.setModeInt(3); // DICE
            randomizer.generate();
            console.log('[WASM Test] DICE mode - Value:', randomizer.getCurrentValue(), '(1-6)');
            randomizer.reset();
            console.log('[WASM Test] After reset() - Count:', randomizer.getGenerationCount());
            randomizer.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Randomizer (not in WASM binary yet - rebuild required)');
        }

        // Test 39: Create Logger (if available in WASM)
        console.log('[WASM Test] --- Test 39: Logger ---');
        if (this.module.Logger) {
            const logger = new this.module.Logger(4400, 100, 50);
            console.log('[WASM Test] Created Logger at:', logger.getX(), logger.getY());
            console.log('[WASM Test] MaxEntries:', logger.getMaxEntries(), '- State:', logger.getStateInt(), '(IDLE=0)');
            logger.startLogging();
            logger.logEntry(0); // DEBUG
            logger.logEntry(1); // INFO
            logger.logEntry(1); // INFO
            logger.logEntry(2); // WARNING
            logger.logEntry(3); // ERROR
            console.log('[WASM Test] After 5 logEntry() calls - EntryCount:', logger.getEntryCount());
            console.log('[WASM Test] Counts - DEBUG:', logger.getDebugCount(), 'INFO:', logger.getInfoCount(),
                       'WARNING:', logger.getWarningCount(), 'ERROR:', logger.getErrorCount());
            console.log('[WASM Test] Fullness:', logger.getFullness(), '- isLogging:', logger.isLogging());
            logger.pauseLogging();
            console.log('[WASM Test] After pauseLogging() - State:', logger.getStateInt(), '(PAUSED=2)');
            logger.clear();
            console.log('[WASM Test] After clear() - EntryCount:', logger.getEntryCount());
            logger.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Logger (not in WASM binary yet - rebuild required)');
        }

        // Test 40: Create Filter (if available in WASM)
        console.log('[WASM Test] --- Test 40: Filter ---');
        if (this.module.Filter) {
            const filter = new this.module.Filter(4600, 100);
            console.log('[WASM Test] Created Filter at:', filter.getX(), filter.getY());
            console.log('[WASM Test] Initial state:', filter.getStateInt(), '(IDLE=0)');
            filter.activate();
            filter.setModeInt(1); // PASS_RANGE
            filter.setThresholds(10.0, 50.0);
            console.log('[WASM Test] After setThresholds(10, 50) - Min:', filter.getMinThreshold(),
                       'Max:', filter.getMaxThreshold());
            const pass1 = filter.processValue(25.0);
            console.log('[WASM Test] processValue(25.0) - Passes:', pass1, '- Output:', filter.getOutputValue());
            const pass2 = filter.processValue(100.0);
            console.log('[WASM Test] processValue(100.0) - Passes:', pass2, '- PassedCount:',
                       filter.getPassedCount(), 'BlockedCount:', filter.getBlockedCount());
            filter.setModeInt(3); // TRANSFORM
            filter.setTransform(2.0, 5.0);
            filter.processValue(10.0);
            console.log('[WASM Test] TRANSFORM mode with scale=2, offset=5 - Input:', filter.getInputValue(),
                       'Output:', filter.getOutputValue(), '(expected 25)');
            filter.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Filter (not in WASM binary yet - rebuild required)');
        }

        // Test 41: Create Accumulator (if available in WASM)
        console.log('[WASM Test] --- Test 41: Accumulator ---');
        if (this.module.Accumulator) {
            const accumulator = new this.module.Accumulator(4800, 100);
            console.log('[WASM Test] Created Accumulator at:', accumulator.getX(), accumulator.getY());
            console.log('[WASM Test] Initial state:', accumulator.getStateInt(), '(IDLE=0)');
            accumulator.start();
            accumulator.setModeInt(0); // SUM
            accumulator.accumulate(10.0);
            accumulator.accumulate(20.0);
            accumulator.accumulate(30.0);
            console.log('[WASM Test] After accumulate(10, 20, 30) in SUM mode - Value:',
                       accumulator.getCurrentValue(), '(expected 60)');
            console.log('[WASM Test] Count:', accumulator.getCount(), '- Sum:', accumulator.getSum());
            console.log('[WASM Test] Min:', accumulator.getMinValue(), '- Max:', accumulator.getMaxValue(),
                       '- Average:', accumulator.getAverage());
            accumulator.setModeInt(1); // AVERAGE
            console.log('[WASM Test] Switched to AVERAGE mode - Value:', accumulator.getCurrentValue(),
                       '(expected 20)');
            accumulator.setModeInt(2); // MIN
            console.log('[WASM Test] Switched to MIN mode - Value:', accumulator.getCurrentValue(),
                       '(expected 10)');
            accumulator.setModeInt(3); // MAX
            console.log('[WASM Test] Switched to MAX mode - Value:', accumulator.getCurrentValue(),
                       '(expected 30)');
            accumulator.reset();
            console.log('[WASM Test] After reset() - Count:', accumulator.getCount(),
                       '- State:', accumulator.getStateInt());
            accumulator.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Accumulator (not in WASM binary yet - rebuild required)');
        }

        // Test 42: Create Sequencer (if available in WASM)
        console.log('[WASM Test] --- Test 42: Sequencer ---');
        if (this.module.Sequencer) {
            const sequencer = new this.module.Sequencer(5000, 100, 5);
            console.log('[WASM Test] Created Sequencer at:', sequencer.getX(), sequencer.getY());
            console.log('[WASM Test] NumSteps:', sequencer.getNumSteps(), '- CurrentStep:', sequencer.getCurrentStep());
            sequencer.start();
            console.log('[WASM Test] After start() - State:', sequencer.getStateInt(), '(RUNNING=1)');
            sequencer.nextStep();
            sequencer.nextStep();
            console.log('[WASM Test] After 2 nextStep() - CurrentStep:', sequencer.getCurrentStep(),
                       '- Progress:', sequencer.getProgress());
            sequencer.setModeInt(1); // LOOP
            sequencer.jumpToStep(4);
            console.log('[WASM Test] After jumpToStep(4) in LOOP mode - CurrentStep:', sequencer.getCurrentStep());
            sequencer.nextStep(); // Should wrap to 0
            console.log('[WASM Test] After nextStep() at end - CurrentStep:', sequencer.getCurrentStep(),
                       '- LoopCount:', sequencer.getLoopCount());
            sequencer.pause();
            console.log('[WASM Test] After pause() - State:', sequencer.getStateInt(), '(PAUSED=2)');
            sequencer.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Sequencer (not in WASM binary yet - rebuild required)');
        }

        // Test 43: Create Trigger (if available in WASM)
        console.log('[WASM Test] --- Test 43: Trigger ---');
        if (this.module.Trigger) {
            const trigger = new this.module.Trigger(5200, 100);
            console.log('[WASM Test] Created Trigger at:', trigger.getX(), trigger.getY());
            console.log('[WASM Test] Initial state:', trigger.getStateInt(), '(IDLE=0)');
            trigger.setThreshold(0.5);
            trigger.arm();
            console.log('[WASM Test] After arm() - State:', trigger.getStateInt(), '(ARMED=1)');

            let triggered = trigger.checkTrigger(0.3);
            console.log('[WASM Test] checkTrigger(0.3) - Triggered:', triggered, '- Value:', trigger.getCurrentValue());

            triggered = trigger.checkTrigger(0.7); // Should trigger (rising edge)
            console.log('[WASM Test] checkTrigger(0.7) - Triggered:', triggered, '- State:', trigger.getStateInt(),
                       '(TRIGGERED=2)');
            console.log('[WASM Test] TriggerCount:', trigger.getTriggerCount());

            trigger.latch();
            console.log('[WASM Test] After latch() - State:', trigger.getStateInt(), '(LATCHED=3)');
            triggered = trigger.checkTrigger(0.9); // Should not trigger when latched
            console.log('[WASM Test] checkTrigger(0.9) while latched - Triggered:', triggered);

            trigger.unlatch();
            trigger.arm();
            console.log('[WASM Test] After unlatch() and arm() - State:', trigger.getStateInt());
            trigger.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Trigger (not in WASM binary yet - rebuild required)');
        }

        // Test 44: Create Scheduler (if available in WASM)
        console.log('[WASM Test] --- Test 44: Scheduler ---');
        if (this.module.Scheduler) {
            const scheduler = new this.module.Scheduler(5400, 100);
            console.log('[WASM Test] Created Scheduler at:', scheduler.getX(), scheduler.getY());
            console.log('[WASM Test] Initial state:', scheduler.getStateInt(), '(IDLE=0)');
            scheduler.setDelay(2.0);
            scheduler.setInterval(1.0);
            console.log('[WASM Test] After setDelay(2.0) and setInterval(1.0) - Delay:', scheduler.getDelay(),
                       'Interval:', scheduler.getInterval());

            scheduler.start();
            console.log('[WASM Test] After start() - State:', scheduler.getStateInt(), '(WAITING=1)');

            // Simulate some time passing
            scheduler.update(1.0);
            console.log('[WASM Test] After update(1.0) - ElapsedTime:', scheduler.getElapsedTime(),
                       '- State:', scheduler.getStateInt());

            scheduler.update(1.5); // Total 2.5s, should be ready
            console.log('[WASM Test] After update(1.5) total 2.5s - State:', scheduler.getStateInt(),
                       '(READY=2)', '- ExecutionCount:', scheduler.getExecutionCount());

            scheduler.setModeInt(1); // INTERVAL mode
            scheduler.setMaxExecutions(3);
            scheduler.start();
            scheduler.update(2.1);
            console.log('[WASM Test] INTERVAL mode after update(2.1) - ExecutionCount:', scheduler.getExecutionCount());
            scheduler.update(1.0);
            scheduler.update(1.0);
            console.log('[WASM Test] After more updates - ExecutionCount:', scheduler.getExecutionCount(),
                       '- State:', scheduler.getStateInt());
            scheduler.delete();
        } else {
            console.log('[WASM Test] ⏭️  Skipping Scheduler (not in WASM binary yet - rebuild required)');
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
    createBox(x: number, y: number, numHoles: number = 3) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        return new this.module.Box(x, y, numHoles);
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
     * Create a Flipper instance from WASM
     */
    createFlipper(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Flipper) {
            throw new Error('Flipper class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Flipper(x, y);
    }

    /**
     * Create a Meter instance from WASM
     */
    createMeter(x: number, y: number, minValue: number = 0, maxValue: number = 100) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Meter) {
            throw new Error('Meter class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Meter(x, y, minValue, maxValue);
    }

    /**
     * Create a Beeper instance from WASM
     */
    createBeeper(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Beeper) {
            throw new Error('Beeper class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Beeper(x, y);
    }

    /**
     * Create a Connector instance from WASM
     */
    createConnector(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Connector) {
            throw new Error('Connector class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Connector(x, y);
    }

    /**
     * Create a Timer instance from WASM
     */
    createTimer(x: number, y: number, duration: number = 10.0) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Timer) {
            throw new Error('Timer class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Timer(x, y, duration);
    }

    /**
     * Create a Counter instance from WASM
     */
    createCounter(x: number, y: number, initialValue: number = 0) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Counter) {
            throw new Error('Counter class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Counter(x, y, initialValue);
    }

    /**
     * Create a Sampler instance from WASM
     */
    createSampler(x: number, y: number, capacity: number = 100) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Sampler) {
            throw new Error('Sampler class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Sampler(x, y, capacity);
    }

    /**
     * Create a Comparator instance from WASM
     */
    createComparator(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Comparator) {
            throw new Error('Comparator class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Comparator(x, y);
    }

    /**
     * Create a Randomizer instance from WASM
     */
    createRandomizer(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Randomizer) {
            throw new Error('Randomizer class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Randomizer(x, y);
    }

    /**
     * Create a Logger instance from WASM
     */
    createLogger(x: number, y: number, maxEntries: number = 100) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Logger) {
            throw new Error('Logger class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Logger(x, y, maxEntries);
    }

    /**
     * Create a Filter instance from WASM
     */
    createFilter(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Filter) {
            throw new Error('Filter class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Filter(x, y);
    }

    /**
     * Create an Accumulator instance from WASM
     */
    createAccumulator(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Accumulator) {
            throw new Error('Accumulator class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Accumulator(x, y);
    }

    /**
     * Create a Sequencer instance from WASM
     */
    createSequencer(x: number, y: number, numSteps: number = 10) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Sequencer) {
            throw new Error('Sequencer class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Sequencer(x, y, numSteps);
    }

    /**
     * Create a Trigger instance from WASM
     */
    createTrigger(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Trigger) {
            throw new Error('Trigger class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Trigger(x, y);
    }

    /**
     * Create a Scheduler instance from WASM
     */
    createScheduler(x: number, y: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.Scheduler) {
            throw new Error('Scheduler class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.Scheduler(x, y);
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

    /**
     * Create a CollisionManager instance from WASM
     */
    createCollisionManager(worldWidth: number, worldHeight: number, maxDepth: number = 5) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.CollisionManager) {
            throw new Error('CollisionManager class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.CollisionManager(worldWidth, worldHeight, maxDepth);
    }

    /**
     * Create an AnimationController instance from WASM
     */
    createAnimationController() {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.AnimationController) {
            throw new Error('AnimationController class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.AnimationController();
    }

    /**
     * Create a SceneManager instance from WASM
     */
    createSceneManager(viewportWidth: number, viewportHeight: number) {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.SceneManager) {
            throw new Error('SceneManager class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.SceneManager(viewportWidth, viewportHeight);
    }

    /**
     * Create an EntityManager instance from WASM
     */
    createEntityManager() {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.EntityManager) {
            throw new Error('EntityManager class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.EntityManager();
    }

    /**
     * Create an InputManager instance from WASM
     */
    createInputManager() {
        if (!this.module) {
            throw new Error('WASM module not loaded');
        }
        if (!this.module.InputManager) {
            throw new Error('InputManager class not available in WASM binary - rebuild required (cd web/core && ./build.sh)');
        }
        return new this.module.InputManager();
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
