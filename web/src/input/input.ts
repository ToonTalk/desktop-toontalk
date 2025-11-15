/**
 * InputManager - Handles all input events
 *
 * Replaces DirectInput from the C++ version.
 * Supports mouse, keyboard, and touch events.
 */

import { ToonTalkRenderer } from '../renderer/renderer';
import { WasmSpriteView } from '../renderer/wasm-sprite-view';

export interface Point {
    x: number;
    y: number;
}

export class InputManager {
    private canvas?: HTMLCanvasElement;
    private renderer?: ToonTalkRenderer;
    private mousePosition: Point = { x: 0, y: 0 };
    private keysPressed: Set<string> = new Set();
    private draggedSprite: WasmSpriteView | null = null;

    initialize(canvas: HTMLCanvasElement, renderer?: ToonTalkRenderer): void {
        this.canvas = canvas;
        this.renderer = renderer;
        console.log('[InputManager] Initializing...', {
            canvas: canvas ? 'present' : 'missing',
            renderer: renderer ? 'present' : 'missing',
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
            console.log('[InputManager] Dragging sprite to', this.mousePosition);
            this.draggedSprite.getWasmSprite().updateDrag(this.mousePosition.x, this.mousePosition.y);
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

        console.log('[InputManager] Mouse down at', this.mousePosition);

        // Find sprite at mouse position
        if (this.renderer) {
            console.log('[InputManager] Looking for sprite at position');
            const sprite = this.renderer.findSpriteAt(this.mousePosition.x, this.mousePosition.y);
            if (sprite) {
                console.log('[InputManager] Found sprite! Starting drag');
                this.draggedSprite = sprite;
                sprite.getWasmSprite().startDrag(this.mousePosition.x, this.mousePosition.y);
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

        if (this.draggedSprite) {
            // End the drag
            this.draggedSprite.getWasmSprite().endDrag();

            // Check for drop target
            if (this.renderer) {
                const dropTarget = this.renderer.findSpriteAt(
                    this.mousePosition.x,
                    this.mousePosition.y,
                    this.draggedSprite
                );

                if (dropTarget) {
                    this.handleDropInteraction(this.draggedSprite, dropTarget);
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
     * Handle drop interactions between sprites
     * Implements ToonTalk-style operations (Number + Number, Text + Text, etc.)
     */
    private handleDropInteraction(draggedSprite: WasmSpriteView, dropTarget: WasmSpriteView): void {
        const dragged = draggedSprite.getWasmSprite();
        const target = dropTarget.getWasmSprite();

        // Check if both are Numbers
        if ('getValue' in dragged && 'getValue' in target) {
            const draggedNum = dragged as any; // Type as any to access Number methods
            const targetNum = target as any;

            // Get the dragged number's value
            const value = draggedNum.getValue();

            // Add the dragged number's value to the target
            // In the future, we'll check if dragged is an operation (e.g., "+ 5")
            // and apply the operation accordingly
            targetNum.add(value);

            console.log(`[Drop] Number ${value} dropped on Number ${targetNum.getValue()}`);
            console.log(`[Drop] Result: ${targetNum.getValue()}`);
        }
        // Check if both are Text
        else if ('getText' in dragged && 'getText' in target) {
            const draggedText = dragged as any;
            const targetText = target as any;

            // Concatenate the dragged text to the target
            const text = draggedText.getText();
            targetText.append(text);

            console.log(`[Drop] Text "${text}" appended to Text "${targetText.getText()}"`);
        }
        // For other types, just log for now
        else {
            console.log('[Drop] Dropped sprite on another sprite (no interaction defined yet)');
        }
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
