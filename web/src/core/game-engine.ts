import { getWasmCore } from './wasm-core';
import type {
    CollisionManager,
    AnimationController,
    SceneManager,
    EntityManager,
    InputManager as WasmInputManager
} from '../types/wasm';

/**
 * GameEngine - Coordinates all WASM game utilities
 *
 * Replicates the original ToonTalk desktop's behavior:
 * - Camera system for world navigation (pan/zoom like original)
 * - Collision detection for drag-and-drop interactions
 * - Smooth animations for object transitions
 * - Entity management for tracking game objects
 * - Gesture recognition for touch/mouse input
 */
export class GameEngine {
    private collision?: CollisionManager;
    private animator?: AnimationController;
    private scene?: SceneManager;
    private entities?: EntityManager;
    private wasmInput?: WasmInputManager;

    private initialized: boolean = false;
    private worldWidth: number = 3200;  // Like original ToonTalk city width
    private worldHeight: number = 2400; // Like original ToonTalk city height

    async initialize(viewportWidth: number, viewportHeight: number): Promise<void> {
        const wasmCore = getWasmCore();

        if (!wasmCore.isLoaded()) {
            throw new Error('WASM core must be initialized before GameEngine');
        }

        console.log('[GameEngine] Initializing game utilities...');

        // Initialize collision manager with world bounds
        this.collision = wasmCore.createCollisionManager(
            this.worldWidth,
            this.worldHeight,
            5  // Max quadtree depth
        );
        console.log('[GameEngine] ✓ Collision system ready');

        // Initialize animation controller for smooth transitions
        this.animator = wasmCore.createAnimationController();
        console.log('[GameEngine] ✓ Animation system ready');

        // Initialize scene manager for camera/viewport
        this.scene = wasmCore.createSceneManager(viewportWidth, viewportHeight);
        // Start camera at center of world (like ToonTalk starting view)
        this.scene.setCameraPosition(this.worldWidth / 2, this.worldHeight / 2);
        this.scene.setCameraZoom(1.0);
        console.log('[GameEngine] ✓ Camera system ready');

        // Initialize entity manager for tracking objects
        this.entities = wasmCore.createEntityManager();
        console.log('[GameEngine] ✓ Entity system ready');

        // Initialize gesture/input manager
        this.wasmInput = wasmCore.createInputManager();
        console.log('[GameEngine] ✓ Gesture recognition ready');

        this.initialized = true;
        console.log('[GameEngine] All systems initialized!');
    }

    isInitialized(): boolean {
        return this.initialized;
    }

    // ===== COLLISION SYSTEM =====

    /**
     * Register an object for collision detection
     * Returns the collision ID for this object
     */
    registerCollider(x: number, y: number, width: number, height: number): number {
        if (!this.collision) throw new Error('Collision system not initialized');
        return this.collision.addObject(x, y, width, height);
    }

    /**
     * Update an object's collision bounds
     */
    updateCollider(id: number, x: number, y: number, width: number, height: number): void {
        if (!this.collision) return;
        this.collision.updateObject(id, x, y, width, height);
    }

    /**
     * Check if two objects are colliding
     */
    checkCollision(id1: number, id2: number): boolean {
        if (!this.collision) return false;
        return this.collision.checkCollision(id1, id2);
    }

    /**
     * Find all objects at a point (for mouse picking)
     */
    findObjectsAtPoint(x: number, y: number): number[] {
        if (!this.collision) return [];
        return this.collision.queryPoint(x, y);
    }

    /**
     * Find all objects in a region (for area selection)
     */
    findObjectsInRegion(x: number, y: number, width: number, height: number): number[] {
        if (!this.collision) return [];
        return this.collision.queryRegion(x, y, width, height);
    }

    /**
     * Cast a ray and find first object hit (for line-of-sight)
     */
    rayCast(startX: number, startY: number, dirX: number, dirY: number, maxDistance: number): number {
        if (!this.collision) return -1;
        return this.collision.rayCast(startX, startY, dirX, dirY, maxDistance);
    }

    // ===== ANIMATION SYSTEM =====

    /**
     * Create a smooth animation (like ToonTalk's smooth movements)
     * Returns animation ID
     */
    createAnimation(startValue: number, endValue: number, duration: number, easingType: number = 0): number {
        if (!this.animator) throw new Error('Animation system not initialized');
        return this.animator.animate(startValue, endValue, duration, easingType);
    }

    /**
     * Get current value of an animation
     */
    getAnimationValue(animId: number): number {
        if (!this.animator) return 0;
        return this.animator.getValue(animId);
    }

    /**
     * Check if animation is still running
     */
    isAnimationActive(animId: number): boolean {
        if (!this.animator) return false;
        return this.animator.isActive(animId);
    }

    /**
     * Stop an animation
     */
    stopAnimation(animId: number): void {
        if (!this.animator) return;
        this.animator.stop(animId);
    }

    // ===== CAMERA/SCENE SYSTEM =====

    /**
     * Move camera to position (like ToonTalk helicopter flying)
     */
    setCameraPosition(x: number, y: number): void {
        if (!this.scene) return;
        this.scene.setCameraPosition(x, y);
    }

    /**
     * Get camera position
     */
    getCameraPosition(): { x: number; y: number } {
        if (!this.scene) return { x: 0, y: 0 };
        return {
            x: this.scene.getCameraX(),
            y: this.scene.getCameraY()
        };
    }

    /**
     * Pan camera by offset (for dragging world)
     */
    panCamera(dx: number, dy: number): void {
        if (!this.scene) return;
        this.scene.moveCamera(dx, dy);
    }

    /**
     * Set zoom level (like ToonTalk's altitude changes)
     */
    setCameraZoom(zoom: number): void {
        if (!this.scene) return;
        this.scene.setCameraZoom(zoom);
    }

    /**
     * Get current zoom level
     */
    getCameraZoom(): number {
        if (!this.scene) return 1.0;
        return this.scene.getCameraZoom();
    }

    /**
     * Convert world coordinates to screen coordinates
     */
    worldToScreen(worldX: number, worldY: number): { x: number; y: number } {
        if (!this.scene) return { x: worldX, y: worldY };

        let screenX = 0, screenY = 0;
        // Note: WASM methods modify passed references, so we need to handle this in TypeScript
        // For now, we'll use the raw Module access
        const module = getWasmCore().getModule();
        if (module && this.scene) {
            // TypeScript doesn't support pass-by-reference for primitives,
            // so we calculate manually
            const cam = this.getCameraPosition();
            const zoom = this.getCameraZoom();
            const vpWidth = this.scene.getViewportWidth();
            const vpHeight = this.scene.getViewportHeight();

            screenX = (worldX - cam.x) * zoom + vpWidth * 0.5;
            screenY = (worldY - cam.y) * zoom + vpHeight * 0.5;
        }

        return { x: screenX, y: screenY };
    }

    /**
     * Convert screen coordinates to world coordinates
     */
    screenToWorld(screenX: number, screenY: number): { x: number; y: number } {
        if (!this.scene) return { x: screenX, y: screenY };

        const cam = this.getCameraPosition();
        const zoom = this.getCameraZoom();
        const vpWidth = this.scene.getViewportWidth();
        const vpHeight = this.scene.getViewportHeight();

        const worldX = (screenX - vpWidth * 0.5) / zoom + cam.x;
        const worldY = (screenY - vpHeight * 0.5) / zoom + cam.y;

        return { x: worldX, y: worldY };
    }

    /**
     * Check if an object is visible in viewport (for culling)
     */
    isVisible(worldX: number, worldY: number, width: number, height: number): boolean {
        if (!this.scene) return true;
        return this.scene.isVisible(worldX, worldY, width, height);
    }

    /**
     * Update viewport size (when window resizes)
     */
    setViewportSize(width: number, height: number): void {
        if (!this.scene) return;
        this.scene.setViewportSize(width, height);
    }

    // ===== ENTITY MANAGEMENT =====

    /**
     * Register a game entity
     */
    createEntity(type: string): number {
        if (!this.entities) throw new Error('Entity system not initialized');
        return this.entities.createEntity(type);
    }

    /**
     * Destroy an entity
     */
    destroyEntity(id: number): void {
        if (!this.entities) return;
        this.entities.destroyEntity(id);
    }

    /**
     * Set entity component data
     */
    setEntityComponent(entityId: number, key: string, value: number): void {
        if (!this.entities) return;
        this.entities.setComponent(entityId, key, value);
    }

    /**
     * Get entity component data
     */
    getEntityComponent(entityId: number, key: string): number {
        if (!this.entities) return 0;
        return this.entities.getComponent(entityId, key);
    }

    /**
     * Query entities by type (e.g., find all "robot" entities)
     */
    getEntitiesByType(type: string): number[] {
        if (!this.entities) return [];
        return this.entities.getEntitiesByType(type);
    }

    // ===== GESTURE/INPUT RECOGNITION =====

    /**
     * Update input system (call each frame)
     */
    updateInput(deltaTime: number): void {
        if (!this.wasmInput) return;
        this.wasmInput.update(deltaTime);
    }

    /**
     * Register pointer down event
     */
    onPointerDown(x: number, y: number): void {
        if (!this.wasmInput) return;
        this.wasmInput.onPointerDown(x, y);
    }

    /**
     * Register pointer up event
     */
    onPointerUp(x: number, y: number): void {
        if (!this.wasmInput) return;
        this.wasmInput.onPointerUp(x, y);
    }

    /**
     * Get last recognized gesture
     */
    getLastGesture(): number {
        if (!this.wasmInput) return 0; // NONE
        return this.wasmInput.getLastGesture();
    }

    /**
     * Clear current gesture
     */
    clearGesture(): void {
        if (!this.wasmInput) return;
        this.wasmInput.clearGesture();
    }

    /**
     * Record a command for replay
     */
    recordCommand(command: string): void {
        if (!this.wasmInput) return;
        this.wasmInput.recordCommand(command);
    }

    // ===== UPDATE LOOP =====

    /**
     * Update all systems (call every frame)
     */
    update(deltaTime: number): void {
        // Update animations
        if (this.animator) {
            this.animator.update(deltaTime / 1000.0); // Convert ms to seconds
        }

        // Update input/gesture recognition
        this.updateInput(deltaTime / 1000.0);
    }

    // ===== CLEANUP =====

    destroy(): void {
        console.log('[GameEngine] Cleaning up...');

        if (this.collision) {
            this.collision.delete();
            this.collision = undefined;
        }

        if (this.animator) {
            this.animator.delete();
            this.animator = undefined;
        }

        if (this.scene) {
            this.scene.delete();
            this.scene = undefined;
        }

        if (this.entities) {
            this.entities.delete();
            this.entities = undefined;
        }

        if (this.wasmInput) {
            this.wasmInput.delete();
            this.wasmInput = undefined;
        }

        this.initialized = false;
        console.log('[GameEngine] Cleanup complete');
    }

    // ===== UTILITY GETTERS =====

    getWorldSize(): { width: number; height: number } {
        return { width: this.worldWidth, height: this.worldHeight };
    }

    getCollisionManager(): CollisionManager | undefined {
        return this.collision;
    }

    getAnimator(): AnimationController | undefined {
        return this.animator;
    }

    getSceneManager(): SceneManager | undefined {
        return this.scene;
    }

    getEntityManager(): EntityManager | undefined {
        return this.entities;
    }

    getInputManager(): WasmInputManager | undefined {
        return this.wasmInput;
    }
}

// Singleton instance
let gameEngineInstance: GameEngine | null = null;

/**
 * Get the GameEngine singleton
 */
export function getGameEngine(): GameEngine {
    if (!gameEngineInstance) {
        gameEngineInstance = new GameEngine();
    }
    return gameEngineInstance;
}
