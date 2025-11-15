/**
 * InputManager - Handles all input events
 *
 * Replaces DirectInput from the C++ version.
 * Supports mouse, keyboard, and touch events.
 */

import { ToonTalkRenderer } from '../renderer/renderer';
import { WasmSpriteView } from '../renderer/wasm-sprite-view';
import { GameEngine } from '../core/game-engine';

export interface Point {
    x: number;
    y: number;
}

export class InputManager {
    private canvas?: HTMLCanvasElement;
    private renderer?: ToonTalkRenderer;
    private gameEngine?: GameEngine;
    private mousePosition: Point = { x: 0, y: 0 };
    private keysPressed: Set<string> = new Set();
    private draggedSprite: WasmSpriteView | null = null;
    private mouseDownTime: number = 0;
    private mouseDownPosition: Point = { x: 0, y: 0 };
    private clickThreshold: number = 200; // milliseconds
    private dragThreshold: number = 5; // pixels

    initialize(canvas: HTMLCanvasElement, renderer?: ToonTalkRenderer, gameEngine?: GameEngine): void {
        this.canvas = canvas;
        this.renderer = renderer;
        this.gameEngine = gameEngine;
        console.log('[InputManager] Initializing...', {
            canvas: canvas ? 'present' : 'missing',
            renderer: renderer ? 'present' : 'missing',
            gameEngine: gameEngine ? 'present' : 'missing',
            canvasSize: canvas ? `${canvas.width}x${canvas.height}` : 'N/A'
        });
        this.setupEventListeners();
        console.log('[InputManager] Input manager initialized successfully');
    }

    private setupEventListeners(): void {
        if (!this.canvas) {
            console.error('[InputManager] Cannot setup listeners - canvas is missing!');
            return;
        }

        console.log('[InputManager] Setting up event listeners on canvas');

        // Mouse events
        this.canvas.addEventListener('mousemove', this.onMouseMove.bind(this));
        this.canvas.addEventListener('mousedown', this.onMouseDown.bind(this));
        this.canvas.addEventListener('mouseup', this.onMouseUp.bind(this));

        // Touch events (for mobile support)
        this.canvas.addEventListener('touchstart', this.onTouchStart.bind(this), { passive: false });
        this.canvas.addEventListener('touchmove', this.onTouchMove.bind(this), { passive: false });
        this.canvas.addEventListener('touchend', this.onTouchEnd.bind(this), { passive: false });

        // Keyboard events
        window.addEventListener('keydown', this.onKeyDown.bind(this));
        window.addEventListener('keyup', this.onKeyUp.bind(this));

        // Prevent context menu on right-click
        this.canvas.addEventListener('contextmenu', (e) => e.preventDefault());

        console.log('[InputManager] Event listeners added successfully');
    }

    private onMouseMove(event: MouseEvent): void {
        this.updateMousePosition(event.clientX, event.clientY);

        // Update dragged sprite position
        if (this.draggedSprite) {
            // Convert screen coordinates to world coordinates
            let worldX = this.mousePosition.x;
            let worldY = this.mousePosition.y;

            if (this.gameEngine) {
                const worldPos = this.gameEngine.screenToWorld(this.mousePosition.x, this.mousePosition.y);
                worldX = worldPos.x;
                worldY = worldPos.y;
            }

            console.log('[InputManager] Dragging sprite to world', worldX.toFixed(2), worldY.toFixed(2));
            this.draggedSprite.getWasmSprite().updateDrag(worldX, worldY);
        }
    }

    private onMouseDown(event: MouseEvent): void {
        console.log('[InputManager] Mouse down event received:', {
            button: event.button,
            clientX: event.clientX,
            clientY: event.clientY,
            target: event.target
        });

        // Only handle left mouse button for dragging (button 0)
        if (event.button !== 0) {
            console.log('[InputManager] Ignoring non-left button');
            return;
        }

        // Record mouse down time and position for click detection
        this.mouseDownTime = Date.now();
        this.mouseDownPosition = { ...this.mousePosition };

        // Convert screen coordinates to world coordinates
        let worldX = this.mousePosition.x;
        let worldY = this.mousePosition.y;

        if (this.gameEngine) {
            const worldPos = this.gameEngine.screenToWorld(this.mousePosition.x, this.mousePosition.y);
            worldX = worldPos.x;
            worldY = worldPos.y;
            console.log(`[InputManager] Mouse down at screen(${this.mousePosition.x.toFixed(2)}, ${this.mousePosition.y.toFixed(2)}) -> world(${worldX.toFixed(2)}, ${worldY.toFixed(2)})`);
        } else {
            console.log('[InputManager] Mouse down at', this.mousePosition, '(no camera transform)');
        }

        // Find sprite at mouse position (in world coordinates)
        if (this.renderer) {
            console.log('[InputManager] Looking for sprite at world position');
            const sprite = this.renderer.findSpriteAt(worldX, worldY);
            if (sprite) {
                console.log('[InputManager] Found sprite! Starting drag');
                this.draggedSprite = sprite;
                sprite.getWasmSprite().startDrag(worldX, worldY);

                // Bring dragged sprite to front
                sprite.bringToFront();
            } else {
                console.log('[InputManager] No sprite found at position');
            }
        } else {
            console.log('[InputManager] Renderer not available');
        }
    }

    private onMouseUp(event: MouseEvent): void {
        console.log('[InputManager] Mouse up event received');
        if (event.button !== 0) return;

        console.log('[InputManager] Mouse up');

        // Calculate time and distance since mouse down
        const timeDelta = Date.now() - this.mouseDownTime;
        const dx = this.mousePosition.x - this.mouseDownPosition.x;
        const dy = this.mousePosition.y - this.mouseDownPosition.y;
        const distance = Math.sqrt(dx * dx + dy * dy);

        // Determine if this was a click or a drag
        const wasClick = timeDelta < this.clickThreshold && distance < this.dragThreshold;

        if (this.draggedSprite) {
            // End the drag
            this.draggedSprite.getWasmSprite().endDrag();

            // Reset z-index
            this.draggedSprite.resetZIndex();

            // Convert screen coordinates to world coordinates
            let worldX = this.mousePosition.x;
            let worldY = this.mousePosition.y;

            if (this.gameEngine) {
                const worldPos = this.gameEngine.screenToWorld(this.mousePosition.x, this.mousePosition.y);
                worldX = worldPos.x;
                worldY = worldPos.y;
            }

            if (wasClick) {
                // Handle click interaction
                console.log('[InputManager] Detected click');
                this.handleClickInteraction(this.draggedSprite);
            } else {
                // Check for drop target (in world coordinates)
                console.log('[InputManager] Detected drag');
                if (this.renderer) {
                    const dropTarget = this.renderer.findSpriteAt(
                        worldX,
                        worldY,
                        this.draggedSprite
                    );

                    if (dropTarget) {
                        this.handleDropInteraction(this.draggedSprite, dropTarget);
                    }
                }
            }

            this.draggedSprite = null;
        }
    }

    private onTouchStart(event: TouchEvent): void {
        event.preventDefault();
        if (event.touches.length > 0) {
            const touch = event.touches[0];
            this.updateMousePosition(touch.clientX, touch.clientY);
            console.log('Touch start at', this.mousePosition);
        }
    }

    private onTouchMove(event: TouchEvent): void {
        event.preventDefault();
        if (event.touches.length > 0) {
            const touch = event.touches[0];
            this.updateMousePosition(touch.clientX, touch.clientY);
        }
    }

    private onTouchEnd(event: TouchEvent): void {
        event.preventDefault();
        console.log('Touch end');
    }

    private onKeyDown(event: KeyboardEvent): void {
        this.keysPressed.add(event.code);
        console.log('Key down:', event.code);
        // TODO: Send to WASM core
    }

    private onKeyUp(event: KeyboardEvent): void {
        this.keysPressed.delete(event.code);
        console.log('Key up:', event.code);
        // TODO: Send to WASM core
    }

    private updateMousePosition(clientX: number, clientY: number): void {
        if (!this.canvas) return;

        const rect = this.canvas.getBoundingClientRect();
        this.mousePosition.x = clientX - rect.left;
        this.mousePosition.y = clientY - rect.top;
    }

    getMousePosition(): Point {
        return { ...this.mousePosition };
    }

    isKeyPressed(code: string): boolean {
        return this.keysPressed.has(code);
    }

    /**
     * Handle click interactions on sprites
     * Implements ToonTalk tool behaviors (Wand, Bomb, Scale, etc.)
     */
    private handleClickInteraction(sprite: WasmSpriteView): void {
        const obj = sprite.getWasmSprite();
        console.log('[Click] Clicked on object');

        // WAND: Cycle through creation modes
        if ('nextMode' in obj && 'getModeInt' in obj) {
            const wand = obj as any;
            wand.nextMode();
            const mode = wand.getModeInt();
            const modes = ['Number', 'Text', 'Box', 'Nest', 'Bird'];
            console.log(`[Click] ✨ Wand mode changed to: ${modes[mode] || mode}`);
            sprite.refresh();
            return;
        }

        // BOMB: Arm/trigger bomb
        if ('arm' in obj && 'getStateInt' in obj) {
            const bomb = obj as any;
            const state = bomb.getStateInt();
            if (state === 0) { // INACTIVE
                bomb.arm();
                console.log('[Click] 💣 Bomb armed! Ticking...');
            } else if (state === 1) { // ARMED
                bomb.explode();
                console.log('[Click] 💥 BOOM! Bomb exploded!');
            } else {
                bomb.reset();
                console.log('[Click] Bomb reset');
            }
            sprite.refresh();
            return;
        }

        // SCALE: Cycle through comparison states
        if ('nextState' in obj && 'getTiltState' in obj) {
            const scale = obj as any;
            scale.nextState();
            const state = scale.getTiltState();
            const states = ['Totter', 'Tilt Left', 'Tilt Right', 'Balanced', 'Frozen', 'Remembering'];
            console.log(`[Click] ⚖️ Scale state: ${states[state] || state}`);
            sprite.refresh();
            return;
        }

        // ROBOT: Toggle run/stop
        if ('start' in obj && 'stop' in obj && 'getStateInt' in obj) {
            const robot = obj as any;
            const state = robot.getStateInt();
            if (state === 0) { // IDLE
                robot.start();
                console.log('[Click] 🤖 Robot started!');
            } else if (state === 1) { // RUNNING
                robot.pause();
                console.log('[Click] 🤖 Robot paused');
            } else {
                robot.stop();
                console.log('[Click] 🤖 Robot stopped');
            }
            sprite.refresh();
            return;
        }

        // NOTEBOOK: Turn page
        if ('nextPage' in obj && 'getCurrentPage' in obj) {
            const notebook = obj as any;
            notebook.nextPage();
            const page = notebook.getCurrentPage();
            console.log(`[Click] 📖 Notebook turned to page ${page}`);
            sprite.refresh();
            return;
        }

        // SENSOR: Toggle active state
        if ('isActive' in obj && 'setActive' in obj && 'getTypeInt' in obj) {
            const sensor = obj as any;
            const active = sensor.isActive();
            sensor.setActive(!active);
            const types = ['Mouse', 'Keyboard', 'Time', 'Collision'];
            const type = types[sensor.getTypeInt()] || 'Unknown';
            console.log(`[Click] 📡 ${type} sensor ${!active ? 'activated' : 'deactivated'}`);
            sprite.refresh();
            return;
        }

        // MOUSE (arithmetic tool): Perform operation
        if ('doOperation' in obj && 'getResult' in obj) {
            const mouse = obj as any;
            mouse.doOperation();
            const result = mouse.getResult();
            console.log(`[Click] 🖱️ Mouse calculation result: ${result}`);
            sprite.refresh();
            return;
        }

        // BOX: Remove/retrieve last item
        if ('removeItem' in obj && 'getCount' in obj) {
            const box = obj as any;
            const count = box.getCount();
            if (count > 0) {
                box.removeItem();
                console.log(`[Click] 📦 Removed item from Box (${box.getCount()}/${box.getNumHoles()} filled)`);
                sprite.refresh();
                // In full ToonTalk, this would create a sprite for the retrieved item
            } else {
                console.log(`[Click] 📦 Box is empty`);
            }
            return;
        }

        // NEST: Clear last occupied hole
        if ('clearHole' in obj && 'countOccupied' in obj) {
            const nest = obj as any;
            const numHoles = nest.getNumHoles();
            // Find last occupied hole and clear it
            for (let i = numHoles - 1; i >= 0; i--) {
                if (!nest.isHoleEmpty(i)) {
                    nest.clearHole(i);
                    console.log(`[Click] 🗂️ Cleared nest hole ${i} (${nest.countOccupied()}/${numHoles} occupied)`);
                    sprite.refresh();
                    // In full ToonTalk, this would create a sprite for the retrieved item
                    break;
                }
            }
            return;
        }

        // NUMBER: Toggle between value and operation mode (future feature)
        // TEXT: Open for editing (future feature)

        console.log('[Click] No click behavior defined for this object type');
    }

    /**
     * Destroy a sprite (remove from renderer and clean up)
     */
    private destroySprite(sprite: WasmSpriteView): void {
        if (this.renderer) {
            this.renderer.removeWasmSprite(sprite);
            console.log('[InputManager] Destroyed sprite');
        }
    }

    /**
     * Handle drop interactions between sprites
     * Implements ToonTalk-style operations (Number + Number, Text + Text, Box storage, etc.)
     */
    private handleDropInteraction(draggedSprite: WasmSpriteView, dropTarget: WasmSpriteView): void {
        const dragged = draggedSprite.getWasmSprite();
        const target = dropTarget.getWasmSprite();

        console.log('[Drop] Checking interaction between objects');

        // NUMBER + NUMBER: Add values (or apply operation if dragged is an operation)
        if ('getValue' in dragged && 'getValue' in target) {
            const draggedNum = dragged as any;
            const targetNum = target as any;
            const value = draggedNum.getValue();

            // Check if dragged number is an operation (has operation property)
            if ('isOperation' in draggedNum && draggedNum.isOperation()) {
                console.log(`[Drop] Applying operation to Number`);
                // Apply the operation to target
                if ('applyOperation' in targetNum) {
                    targetNum.applyOperation(draggedNum.getOperation(), value);
                }
            } else {
                // Simple addition
                targetNum.add(value);
            }

            console.log(`[Drop] Number ${value} → Number = ${targetNum.getValue()}`);

            // Refresh the visual display
            dropTarget.refresh();

            // Destroy the dragged number (it's been consumed)
            this.destroySprite(draggedSprite);
            return;
        }

        // TEXT + TEXT: Concatenate
        if ('getText' in dragged && 'getText' in target) {
            const draggedText = dragged as any;
            const targetText = target as any;
            const text = draggedText.getText();
            targetText.append(text);
            console.log(`[Drop] Text "${text}" → Text = "${targetText.getText()}"`);

            // Refresh the visual display
            dropTarget.refresh();

            // Destroy the dragged text (it's been consumed)
            this.destroySprite(draggedSprite);
            return;
        }

        // ANY + BOX: Store in box hole
        if ('addItem' in target && 'getNumHoles' in target) {
            const box = target as any;
            if (!box.isFull()) {
                box.addItem();
                console.log(`[Drop] Item added to Box (${box.getCount()}/${box.getNumHoles()} filled)`);

                // Refresh box visual to show new item
                dropTarget.refresh();

                // Destroy the dragged item (it's been stored in the box)
                this.destroySprite(draggedSprite);
            } else {
                console.log(`[Drop] Box is full!`);
            }
            return;
        }

        // ANY + NEST: Fill nest hole
        if ('setHole' in target && 'getNumHoles' in target) {
            const nest = target as any;
            if (!nest.isFull()) {
                // Find first empty hole and fill it
                const numHoles = nest.getNumHoles();
                for (let i = 0; i < numHoles; i++) {
                    if (nest.isHoleEmpty(i)) {
                        nest.setHole(i, true);
                        console.log(`[Drop] Filled nest hole ${i} (${nest.countOccupied()}/${numHoles} occupied)`);

                        // Refresh nest visual to show new item
                        dropTarget.refresh();

                        // Destroy the dragged item (it's been stored in the nest)
                        this.destroySprite(draggedSprite);
                        break;
                    }
                }
            } else {
                console.log(`[Drop] Nest is full!`);
            }
            return;
        }

        // NUMBER + NOTEBOOK: Jump to page number
        if ('getValue' in dragged && 'setCurrentPage' in target) {
            const num = dragged as any;
            const notebook = target as any;
            const pageNum = Math.floor(num.getValue());
            const maxPages = notebook.getNumPages();

            if (pageNum >= 0 && pageNum < maxPages) {
                notebook.setCurrentPage(pageNum);
                console.log(`[Drop] Notebook jumped to page ${pageNum}`);
            } else {
                console.log(`[Drop] Page ${pageNum} out of range (0-${maxPages - 1})`);
            }
            return;
        }

        // ANY + NOTEBOOK: Store on current page
        if ('setPageContent' in target && 'getCurrentPage' in target) {
            const notebook = target as any;
            const currentPage = notebook.getCurrentPage();
            notebook.setPageContent(currentPage, true);
            console.log(`[Drop] Content stored on notebook page ${currentPage}`);
            return;
        }

        // NUMBER + SCALE: Update scale (for comparison)
        if ('getValue' in dragged && 'compareNeighbors' in target) {
            const scale = target as any;
            scale.setActive(true);
            console.log(`[Drop] Number placed on Scale for comparison`);
            // In full ToonTalk, scale would compare left vs right values
            return;
        }

        // BOMB + ANY: Destroy/remove target
        if ('getStateInt' in dragged && 'getFuseTime' in dragged) {
            const bomb = dragged as any;
            // Check if this is actually a bomb (has explode method)
            if ('explode' in bomb) {
                console.log(`[Drop] 💣 BOOM! Bomb exploded the target!`);
                bomb.explode();
                // In full ToonTalk, this would remove/destroy the target sprite
                // For now, just trigger the explosion animation
                return;
            }
        }

        // WAND + ANY: Create copy or transform based on wand mode
        if ('getModeInt' in dragged && 'setActive' in dragged) {
            const wand = dragged as any;
            if ('nextMode' in wand) {
                const mode = wand.getModeInt();
                console.log(`[Drop] ✨ Wand used (mode ${mode})! Would create/transform object`);
                // In full ToonTalk, wand would create new objects or transform existing ones
                // Mode 0: CREATE_NUMBER, 1: CREATE_TEXT, 2: CREATE_BOX, 3: CREATE_NEST, 4: CREATE_BIRD
                return;
            }
        }

        // NUMBER + TEXT: Change first/last letter by amount
        if ('getValue' in dragged && 'changeFirstLetter' in target) {
            const num = dragged as any;
            const text = target as any;
            const amount = Math.floor(num.getValue());
            text.changeFirstLetter(amount);
            console.log(`[Drop] Number ${amount} changed first letter of Text`);
            return;
        }

        // MOUSE (arithmetic tool) + NUMBER: Perform operation
        if ('doOperation' in dragged && 'getValue' in target) {
            const mouse = dragged as any;
            const num = target as any;
            if ('getOperand1' in mouse) {
                mouse.setOperand2(num.getValue());
                mouse.doOperation();
                const result = mouse.getResult();
                console.log(`[Drop] Mouse arithmetic result: ${result}`);
                num.setValue(result);
                return;
            }
        }

        // ROBOT: Training mode - record actions
        if ('train' in target && 'getInstructionCount' in target) {
            const robot = target as any;
            console.log(`[Drop] 🤖 Robot is watching and learning this action...`);
            robot.addInstruction();
            console.log(`[Drop] Robot now has ${robot.getInstructionCount()} instructions`);
            return;
        }

        // Default: No specific interaction defined
        console.log('[Drop] No specific interaction defined for these object types');
    }

    destroy(): void {
        // Remove event listeners
        if (this.canvas) {
            this.canvas.removeEventListener('mousemove', this.onMouseMove.bind(this));
            this.canvas.removeEventListener('mousedown', this.onMouseDown.bind(this));
            this.canvas.removeEventListener('mouseup', this.onMouseUp.bind(this));
            this.canvas.removeEventListener('touchstart', this.onTouchStart.bind(this));
            this.canvas.removeEventListener('touchmove', this.onTouchMove.bind(this));
            this.canvas.removeEventListener('touchend', this.onTouchEnd.bind(this));
        }

        window.removeEventListener('keydown', this.onKeyDown.bind(this));
        window.removeEventListener('keyup', this.onKeyUp.bind(this));
    }
}
