import * as PIXI from 'pixi.js';
import type { Sprite, Bird, ToonTalkNumber, ToonTalkText, ToonTalkBox, ToonTalkNest } from '../types/wasm';
import type { ToonTalkRenderer } from './renderer';

/**
 * WasmSpriteView - Bridges WASM Sprite objects with PixiJS rendering
 *
 * This takes a C++ Sprite (or Bird, Number, Text, Box) from WASM and creates a PixiJS
 * visual representation that stays synchronized with the C++ object.
 */
export class WasmSpriteView {
    private wasmSprite: Sprite | Bird | ToonTalkNumber | ToonTalkText | ToonTalkBox | ToonTalkNest;
    private graphics: PIXI.Graphics;
    private textDisplay?: PIXI.Text;
    private destroyed: boolean = false;
    private renderer: ToonTalkRenderer;

    // Drag state
    private isDragging: boolean = false;
    private dragOffset: { x: number; y: number } = { x: 0, y: 0 };
    private originalScale: { x: number; y: number } = { x: 1, y: 1 };
    private isHovered: boolean = false;
    private dropTarget: WasmSpriteView | null = null;
    private dropOnLeftHalf: boolean = true; // Track which half we're dropping on

    constructor(wasmSprite: Sprite | Bird | ToonTalkNumber | ToonTalkText | ToonTalkBox | ToonTalkNest, stage: PIXI.Container, renderer: ToonTalkRenderer) {
        this.wasmSprite = wasmSprite;
        this.graphics = new PIXI.Graphics();
        this.renderer = renderer;

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
     * Detect object type
     */
    private getObjectType(): string {
        if ('setVelocity' in this.wasmSprite) return 'bird';
        if ('getValue' in this.wasmSprite) return 'number';
        if ('getText' in this.wasmSprite) return 'text';
        if ('getNumHoles' in this.wasmSprite) return 'nest';
        if ('getCapacity' in this.wasmSprite) return 'box';
        return 'sprite';
    }

    /**
     * Draw a visual representation of the sprite
     * Different shapes and styles for each object type
     */
    private drawSprite(): void {
        const type = this.getObjectType();

        this.graphics.clear();

        switch (type) {
            case 'bird':
                this.drawBird();
                break;
            case 'number':
                this.drawNumber();
                break;
            case 'text':
                this.drawText();
                break;
            case 'box':
                this.drawBox();
                break;
            case 'nest':
                this.drawNest();
                break;
            default:
                this.drawGenericSprite();
                break;
        }
    }

    private drawBird(): void {
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

        // Wings
        this.graphics.beginFill(0xFFA500, 0.7);
        this.graphics.drawEllipse(-25, 0, 15, 10);
        this.graphics.drawEllipse(25, 0, 15, 10);
        this.graphics.endFill();

        // Label
        const label = new PIXI.Text('Bird', {
            fontSize: 10,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 45;
        this.graphics.addChild(label);
    }

    private drawNumber(): void {
        const num = this.wasmSprite as ToonTalkNumber;

        // Green rounded rectangle
        this.graphics.beginFill(0x32CD32);
        this.graphics.drawRoundedRect(-40, -30, 80, 60, 10);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x228B22);
        this.graphics.drawRoundedRect(-40, -30, 80, 60, 10);

        // Display the number value
        const valueText = new PIXI.Text(num.toString(), {
            fontSize: 24,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 3
        });
        valueText.anchor.set(0.5);
        this.graphics.addChild(valueText);

        // Store for updates
        this.textDisplay = valueText;
    }

    private drawText(): void {
        const txt = this.wasmSprite as ToonTalkText;
        const textContent = txt.getText() || "(empty)";

        // Yellow rounded rectangle
        this.graphics.beginFill(0xFFFF00);
        this.graphics.drawRoundedRect(-50, -20, 100, 40, 8);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(2, 0xFFAA00);
        this.graphics.drawRoundedRect(-50, -20, 100, 40, 8);

        // Display text
        const displayText = new PIXI.Text(textContent, {
            fontSize: 14,
            fill: 0x000000,
            wordWrap: true,
            wordWrapWidth: 90
        });
        displayText.anchor.set(0.5);
        this.graphics.addChild(displayText);

        this.textDisplay = displayText;
    }

    private drawBox(): void {
        const box = this.wasmSprite as ToonTalkBox;
        const fullness = box.getFullness();

        // Brown box shape
        this.graphics.beginFill(0x8B4513);
        this.graphics.drawRect(-60, -50, 120, 100);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x654321);
        this.graphics.drawRect(-60, -50, 120, 100);

        // Fill indicator (green fill level)
        const fillHeight = 90 * fullness;
        this.graphics.beginFill(0x90EE90, 0.6);
        this.graphics.drawRect(-55, 45 - fillHeight, 110, fillHeight);
        this.graphics.endFill();

        // Label at top
        const label = new PIXI.Text('Box', {
            fontSize: 12,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = -35;
        this.graphics.addChild(label);

        // Capacity text at center - make it clearer this is item count
        const text = new PIXI.Text(`${box.getCount()} items`, {
            fontSize: 14,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        text.anchor.set(0.5);
        text.y = -5;
        this.graphics.addChild(text);

        // Capacity at bottom
        const capacityText = new PIXI.Text(`(max ${box.getCapacity()})`, {
            fontSize: 10,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 2
        });
        capacityText.anchor.set(0.5);
        capacityText.y = 15;
        this.graphics.addChild(capacityText);

        this.textDisplay = text;
    }

    private drawNest(): void {
        const nest = this.wasmSprite as ToonTalkNest;
        const numHoles = nest.getNumHoles();
        const occupied = nest.countOccupied();

        // Purple nest container
        this.graphics.beginFill(0x9370DB);
        this.graphics.drawRoundedRect(-70, -45, 140, 90, 8);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x663399);
        this.graphics.drawRoundedRect(-70, -45, 140, 90, 8);

        // Label at top
        const label = new PIXI.Text('Nest', {
            fontSize: 12,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = -35;
        this.graphics.addChild(label);

        // Draw holes
        const holeSpacing = 120 / (numHoles + 1);
        for (let i = 0; i < numHoles; i++) {
            const holeX = -60 + (i + 1) * holeSpacing;
            const holeY = 5;
            const isEmpty = nest.isHoleEmpty(i);

            // Draw hole
            if (isEmpty) {
                // Empty hole - dark circle
                this.graphics.beginFill(0x4B0082, 0.6);
            } else {
                // Occupied hole - light circle
                this.graphics.beginFill(0xFFD700);
            }
            this.graphics.drawCircle(holeX, holeY, 12);
            this.graphics.endFill();

            // Hole border
            this.graphics.lineStyle(2, isEmpty ? 0x663399 : 0xFFD700);
            this.graphics.drawCircle(holeX, holeY, 12);
        }

        // Status text
        const statusText = new PIXI.Text(`${occupied} / ${numHoles}`, {
            fontSize: 14,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        statusText.anchor.set(0.5);
        statusText.y = 30;
        this.graphics.addChild(statusText);

        this.textDisplay = statusText;
    }

    private drawGenericSprite(): void {
        // Generic blue square
        this.graphics.beginFill(0x4169E1);
        this.graphics.drawRect(-25, -25, 50, 50);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(2, 0xFFFFFF);
        this.graphics.drawRect(-25, -25, 50, 50);

        const label = new PIXI.Text('Sprite', {
            fontSize: 10,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 45;
        this.graphics.addChild(label);
    }

    /**
     * Set up mouse interactions
     */
    private setupInteractions(): void {
        // Hover effects
        this.graphics.on('pointerover', () => {
            if (!this.isDragging) {
                this.isHovered = true;
                this.graphics.scale.set(1.1);
            }
        });

        this.graphics.on('pointerout', () => {
            if (!this.isDragging) {
                this.isHovered = false;
                this.graphics.scale.set(1);
            }
        });

        // Start drag
        this.graphics.on('pointerdown', (event: PIXI.FederatedPointerEvent) => {
            this.startDrag(event);
        });
    }

    /**
     * Start dragging the sprite
     */
    private startDrag(event: PIXI.FederatedPointerEvent): void {
        this.isDragging = true;
        this.originalScale = { x: this.graphics.scale.x, y: this.graphics.scale.y };

        // Calculate drag offset (where on the sprite did we click?)
        const spritePos = this.graphics.position;
        this.dragOffset.x = spritePos.x - event.global.x;
        this.dragOffset.y = spritePos.y - event.global.y;

        // Visual feedback - lift the sprite
        this.graphics.scale.set(1.15);
        this.graphics.alpha = 0.8;

        // Move to front (higher z-index)
        if (this.graphics.parent) {
            this.graphics.parent.setChildIndex(this.graphics, this.graphics.parent.children.length - 1);
        }

        // Set up global move and up handlers
        const onPointerMove = (e: PIXI.FederatedPointerEvent) => {
            this.onDrag(e);
        };

        const onPointerUp = () => {
            this.stopDrag();
            this.graphics.off('globalpointermove', onPointerMove);
            this.graphics.off('pointerup', onPointerUp);
            this.graphics.off('pointerupoutside', onPointerUp);
        };

        this.graphics.on('globalpointermove', onPointerMove);
        this.graphics.on('pointerup', onPointerUp);
        this.graphics.on('pointerupoutside', onPointerUp);

        console.log('[WasmSpriteView] Started dragging');
    }

    /**
     * Handle dragging movement
     */
    private onDrag(event: PIXI.FederatedPointerEvent): void {
        if (!this.isDragging) return;

        // Calculate new sprite position based on mouse + offset
        const newX = event.global.x + this.dragOffset.x;
        const newY = event.global.y + this.dragOffset.y;

        // Update WASM sprite position
        this.wasmSprite.setPosition(newX, newY);

        // UX IMPROVEMENT: Use MOUSE position for collision detection
        // Original ToonTalk uses sprite centers (see source/text.cpp:991-993)
        // but using mouse position is more intuitive for web users
        const mouseX = event.global.x;
        const mouseY = event.global.y;
        const targetUnderMouse = this.renderer.findSpriteAt(mouseX, mouseY, this);

        // Clear old drop target highlight
        if (this.dropTarget && this.dropTarget !== targetUnderMouse) {
            this.clearDropTargetHighlight(this.dropTarget);
            this.dropTarget = null;
        }

        // Highlight new drop target
        if (targetUnderMouse && targetUnderMouse.canAcceptDrop(this)) {
            this.dropTarget = targetUnderMouse;

            // UX IMPROVEMENT: Check if MOUSE (not sprite center) is left/right of target center
            // More intuitive: where you point determines the result, not where sprite center lands
            const targetX = targetUnderMouse.getWasmSprite().getX();
            this.dropOnLeftHalf = (mouseX < targetX);

            this.highlightDropTarget(this.dropTarget, this.dropOnLeftHalf);
        } else {
            this.dropTarget = null;
        }
    }

    /**
     * Stop dragging the sprite
     */
    private stopDrag(): void {
        if (!this.isDragging) return;

        // Handle drop interaction if there's a valid target
        if (this.dropTarget) {
            this.dropTarget.handleDrop(this, this.dropOnLeftHalf);
            this.clearDropTargetHighlight(this.dropTarget);
            this.dropTarget = null;

            // Note: if handleDrop removes this sprite, we'll be destroyed
            if (this.destroyed) return;
        }

        this.isDragging = false;

        // Reset visual feedback
        if (this.isHovered) {
            this.graphics.scale.set(1.1);
        } else {
            this.graphics.scale.set(1);
        }
        this.graphics.alpha = 1.0;

        console.log('[WasmSpriteView] Stopped dragging at',
            this.wasmSprite.getX(), this.wasmSprite.getY());
    }

    /**
     * Highlight a drop target
     */
    private highlightDropTarget(target: WasmSpriteView, isLeftHalf: boolean): void {
        const targetGraphics = target.getGraphics();
        // Different tint for left vs right half
        // Left half: Green (0x00FF00), Right half: Cyan (0x00FFFF)
        targetGraphics.tint = isLeftHalf ? 0x00FF00 : 0x00FFFF;
        targetGraphics.alpha = 0.7;
    }

    /**
     * Clear drop target highlight
     */
    private clearDropTargetHighlight(target: WasmSpriteView): void {
        const targetGraphics = target.getGraphics();
        targetGraphics.tint = 0xFFFFFF; // Reset tint
        targetGraphics.alpha = 1.0;
    }

    /**
     * Update the PixiJS graphics to match the WASM sprite state
     * Call this every frame!
     */
    update(deltaTime: number): void {
        if (this.destroyed) return;

        // Update WASM sprite logic (only if not dragging, to avoid interference)
        if (!this.isDragging) {
            this.wasmSprite.update(deltaTime);
        }

        // Sync position from WASM to PixiJS (but not while dragging!)
        if (!this.isDragging) {
            this.graphics.x = this.wasmSprite.getX();
            this.graphics.y = this.wasmSprite.getY();
        } else {
            // While dragging, keep PixiJS and WASM in sync
            this.graphics.x = this.wasmSprite.getX();
            this.graphics.y = this.wasmSprite.getY();
        }

        // Sync rotation
        this.graphics.rotation = this.wasmSprite.getRotation();

        // Sync visibility
        this.graphics.visible = this.wasmSprite.isVisible();

        // Update dynamic text displays
        const type = this.getObjectType();
        if (type === 'number' && this.textDisplay) {
            const num = this.wasmSprite as ToonTalkNumber;
            this.textDisplay.text = num.toString();
        } else if (type === 'text' && this.textDisplay) {
            const txt = this.wasmSprite as ToonTalkText;
            this.textDisplay.text = txt.getText() || "(empty)";
        } else if (type === 'box' && this.textDisplay) {
            const box = this.wasmSprite as ToonTalkBox;
            this.textDisplay.text = `${box.getCount()} items`;

            // Also update the fill level visual
            this.drawBox(); // Redraw to show new fill level
        }
    }

    /**
     * Get the underlying WASM sprite
     */
    getWasmSprite(): Sprite | Bird | ToonTalkNumber | ToonTalkText | ToonTalkBox {
        return this.wasmSprite;
    }

    /**
     * Get the PixiJS graphics object
     */
    getGraphics(): PIXI.Graphics {
        return this.graphics;
    }

    /**
     * Check if a point is inside this sprite
     */
    containsPoint(x: number, y: number): boolean {
        return this.wasmSprite.containsPoint(x, y);
    }

    /**
     * Check if this sprite can accept being dropped on by another sprite
     */
    canAcceptDrop(other: WasmSpriteView): boolean {
        const myType = this.getObjectType();
        const otherType = other.getObjectType();

        // Number can accept Number (addition)
        if (myType === 'number' && otherType === 'number') return true;

        // Text can accept Text (concatenation)
        if (myType === 'text' && otherType === 'text') return true;

        // Box can accept any object (storage)
        if (myType === 'box') return true;

        return false;
    }

    /**
     * Handle an object being dropped onto this sprite
     * @param droppedSprite The sprite being dropped
     * @param droppedOnLeftHalf True if dropped on left half, false if right half
     *
     * ToonTalk pattern:
     * - Drop A on LEFT half of B → result is AB (A comes first)
     * - Drop A on RIGHT half of B → result is BA (B comes first)
     */
    handleDrop(droppedSprite: WasmSpriteView, droppedOnLeftHalf: boolean): void {
        const myType = this.getObjectType();
        const droppedType = droppedSprite.getObjectType();

        const side = droppedOnLeftHalf ? 'left' : 'right';
        console.log(`[Drop] ${droppedType} onto ${side} half of ${myType}`);

        // Number + Number = Addition
        if (myType === 'number' && droppedType === 'number') {
            const myNum = this.wasmSprite as ToonTalkNumber;
            const droppedNum = droppedSprite.wasmSprite as ToonTalkNumber;

            const myValue = myNum.getValue();
            const droppedValue = droppedNum.getValue();
            const result = myValue + droppedValue;

            myNum.setValue(result);

            if (droppedOnLeftHalf) {
                console.log(`✨ ${droppedValue} + ${myValue} = ${result} (dropped on left)`);
            } else {
                console.log(`✨ ${myValue} + ${droppedValue} = ${result} (dropped on right)`);
            }

            // Remove the dropped number
            this.renderer.removeWasmSprite(droppedSprite);
        }

        // Text + Text = Concatenation (ORDER MATTERS!)
        else if (myType === 'text' && droppedType === 'text') {
            const myText = this.wasmSprite as ToonTalkText;
            const droppedText = droppedSprite.wasmSprite as ToonTalkText;

            const myString = myText.getText();
            const droppedString = droppedText.getText();
            let result: string;

            if (droppedOnLeftHalf) {
                // Dropped on LEFT: dropped comes first
                result = droppedString + myString;
                console.log(`✨ "${droppedString}" + "${myString}" = "${result}" (dropped on left)`);
            } else {
                // Dropped on RIGHT: target comes first
                result = myString + droppedString;
                console.log(`✨ "${myString}" + "${droppedString}" = "${result}" (dropped on right)`);
            }

            myText.setText(result);

            // Remove the dropped text
            this.renderer.removeWasmSprite(droppedSprite);
        }

        // Box accepts anything
        else if (myType === 'box') {
            const box = this.wasmSprite as ToonTalkBox;

            if (!box.isFull()) {
                box.addItem();
                console.log(`✨ Added item to box (${box.getCount()} items)`);

                // Remove the dropped object
                this.renderer.removeWasmSprite(droppedSprite);
            } else {
                console.log('❌ Box is full!');
            }
        }
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
