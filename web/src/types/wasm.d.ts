/**
 * Type definitions for ToonTalk WASM Core
 *
 * These types match the C++ classes and functions exposed via Emscripten bindings
 */

export interface ToonTalkCoreModule extends EmscriptenModule {
    // Core functions
    getVersion(): string;
    initialize(): boolean;
    add(a: number, b: number): number;
    greet(name: string): string;

    // Classes
    Sprite: typeof Sprite;
    Bird: typeof Bird;
    Number: typeof ToonTalkNumber;
    Text: typeof ToonTalkText;
    Box: typeof ToonTalkBox;
    Nest: typeof ToonTalkNest;
}

/**
 * Sprite class (matches sprite.cpp)
 */
export declare class Sprite {
    constructor(x: number, y: number, width: number, height: number);

    getX(): number;
    getY(): number;
    setX(x: number): void;
    setY(y: number): void;
    setPosition(x: number, y: number): void;

    getWidth(): number;
    getHeight(): number;
    setWidth(w: number): void;
    setHeight(h: number): void;

    getRotation(): number;
    setRotation(r: number): void;
    rotate(dr: number): void;

    isVisible(): boolean;
    setVisible(v: boolean): void;

    update(deltaTime: number): void;
    containsPoint(x: number, y: number): boolean;

    delete(): void;
}

/**
 * Bird class (matches sprite.cpp)
 */
export declare class Bird extends Sprite {
    constructor(x: number, y: number);
    setVelocity(vx: number, vy: number): void;
}

/**
 * Number class (matches objects.cpp)
 */
export declare class ToonTalkNumber extends Sprite {
    constructor(x: number, y: number, value?: number);
    getValue(): number;
    setValue(v: number): void;
    add(amount: number): void;
    subtract(amount: number): void;
    multiply(amount: number): void;
    divide(amount: number): void;
    toString(): string;
    delete(): void;
}

/**
 * Text class (matches objects.cpp)
 */
export declare class ToonTalkText extends Sprite {
    constructor(x: number, y: number, text?: string);
    getText(): string;
    setText(text: string): void;
    append(str: string): void;
    clear(): void;
    length(): number;
    delete(): void;
}

/**
 * Box class (matches objects.cpp)
 */
export declare class ToonTalkBox extends Sprite {
    constructor(x: number, y: number, capacity?: number);
    getCapacity(): number;
    getCount(): number;
    isEmpty(): boolean;
    isFull(): boolean;
    addItem(): boolean;
    removeItem(): boolean;
    clear(): void;
    getFullness(): number;
    delete(): void;
}

/**
 * Nest class (matches objects.cpp) - Container with holes
 */
export declare class ToonTalkNest extends Sprite {
    constructor(x: number, y: number, numHoles?: number);
    getNumHoles(): number;
    isHoleEmpty(index: number): boolean;
    setHole(index: number, occupied: boolean): void;
    clearHole(index: number): void;
    clearAll(): void;
    countOccupied(): number;
    isFull(): boolean;
    isEmpty(): boolean;
    delete(): void;
}

/**
 * Emscripten Module interface
 */
export interface EmscriptenModule {
    onRuntimeInitialized?: () => void;
    print?: (text: string) => void;
    printErr?: (text: string) => void;
    locateFile?: (path: string, prefix: string) => string;
}

/**
 * Factory function type (created by Emscripten)
 */
export type CreateToonTalkCore = (module?: Partial<EmscriptenModule>) => Promise<ToonTalkCoreModule>;

declare global {
    interface Window {
        createToonTalkCore?: CreateToonTalkCore;
    }
}
