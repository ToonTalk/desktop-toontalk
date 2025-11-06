/**
 * InputManager - Handles all input events
 *
 * Replaces DirectInput from the C++ version.
 * Supports mouse, keyboard, and touch events.
 */

export interface Point {
    x: number;
    y: number;
}

export class InputManager {
    private canvas?: HTMLCanvasElement;
    private mousePosition: Point = { x: 0, y: 0 };
    private keysPressed: Set<string> = new Set();

    initialize(canvas: HTMLCanvasElement): void {
        this.canvas = canvas;
        this.setupEventListeners();
        console.log('Input manager initialized');
    }

    private setupEventListeners(): void {
        if (!this.canvas) return;

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
    }

    private onMouseMove(event: MouseEvent): void {
        this.updateMousePosition(event.clientX, event.clientY);
    }

    private onMouseDown(event: MouseEvent): void {
        console.log('Mouse down:', event.button, 'at', this.mousePosition);
        // TODO: Send to WASM core
    }

    private onMouseUp(event: MouseEvent): void {
        console.log('Mouse up:', event.button);
        // TODO: Send to WASM core
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
