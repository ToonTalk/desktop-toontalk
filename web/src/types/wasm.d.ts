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
    Scale: typeof ToonTalkScale;
    Wand: typeof ToonTalkWand;
    Robot: typeof ToonTalkRobot;
    House: typeof ToonTalkHouse;
    Truck: typeof ToonTalkTruck;
    Picture: typeof ToonTalkPicture;
    Sensor: typeof ToonTalkSensor;
    Notebook: typeof ToonTalkNotebook;
    Bomb: typeof ToonTalkBomb;
    Thought_Bubble: typeof ToonTalkThoughtBubble;
    Mouse: typeof ToonTalkMouse;

    // Enums
    WandMode: typeof WandMode;
    RobotState: typeof RobotState;
    TruckState: typeof TruckState;
    SensorType: typeof SensorType;
    BombState: typeof BombState;
    MouseState: typeof MouseState;
    MouseOperation: typeof MouseOperation;
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
 * Scale class (matches objects.cpp) - Copy tool
 */
export declare class ToonTalkScale extends Sprite {
    constructor(x: number, y: number);
    isActive(): boolean;
    setActive(active: boolean): void;
    delete(): void;
}

/**
 * Wand class (matches objects.cpp) - Magic wand for creating objects
 */
export declare class ToonTalkWand extends Sprite {
    constructor(x: number, y: number, mode?: WandMode);
    getModeInt(): number;
    setModeInt(mode: number): void;
    isActive(): boolean;
    setActive(active: boolean): void;
    nextMode(): void;
    previousMode(): void;
    delete(): void;
}

/**
 * Wand mode enum (matches objects.cpp)
 */
export enum WandMode {
    CREATE_NUMBER = 0,
    CREATE_TEXT = 1,
    CREATE_BOX = 2,
    CREATE_NEST = 3,
    CREATE_BIRD = 4
}

/**
 * Robot class (matches objects.cpp) - Programmable agent
 */
export declare class ToonTalkRobot extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    getInstructionCount(): number;
    setInstructionCount(count: number): void;
    addInstruction(): void;
    clearInstructions(): void;
    start(): void;
    stop(): void;
    pause(): void;
    train(): void;
    delete(): void;
}

/**
 * Robot state enum (matches objects.cpp)
 */
export enum RobotState {
    IDLE = 0,
    RUNNING = 1,
    PAUSED = 2,
    TRAINING = 3
}

/**
 * House class (matches objects.cpp) - Workspace container
 */
export declare class ToonTalkHouse extends Sprite {
    constructor(x: number, y: number, numRooms?: number);
    getNumRooms(): number;
    getCurrentRoom(): number;
    setCurrentRoom(room: number): void;
    isRoomOccupied(room: number): boolean;
    setRoomOccupied(room: number, occupied: boolean): void;
    countOccupiedRooms(): number;
    isFull(): boolean;
    isEmpty(): boolean;
    delete(): void;
}

/**
 * Truck class (matches objects.cpp) - Delivery/transport vehicle
 */
export declare class ToonTalkTruck extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    hasCargo(): boolean;
    setCargo(hasCargo: boolean): void;
    startDelivery(): void;
    completeDelivery(): void;
    delete(): void;
}

/**
 * Truck state enum (matches objects.cpp)
 */
export enum TruckState {
    EMPTY = 0,
    LOADED = 1,
    DELIVERING = 2
}

/**
 * Picture class (matches objects.cpp) - Image display
 */
export declare class ToonTalkPicture extends Sprite {
    constructor(x: number, y: number, width?: number, height?: number);
    getPictureWidth(): number;
    getPictureHeight(): number;
    setPictureSize(width: number, height: number): void;
    hasImage(): boolean;
    setHasImage(hasImage: boolean): void;
    getImageId(): number;
    setImageId(id: number): void;
    delete(): void;
}

/**
 * Sensor class (matches objects.cpp) - Input and sensor functionality
 */
export declare class ToonTalkSensor extends Sprite {
    constructor(x: number, y: number, type?: SensorType);
    getTypeInt(): number;
    setTypeInt(type: number): void;
    isActive(): boolean;
    setActive(active: boolean): void;
    getValue(): number;
    setValue(value: number): void;
    delete(): void;
}

/**
 * Sensor type enum (matches objects.cpp)
 */
export enum SensorType {
    MOUSE = 0,
    KEYBOARD = 1,
    TIME = 2,
    COLLISION = 3
}

/**
 * Notebook class (matches objects.cpp) - Notes and documentation
 */
export declare class ToonTalkNotebook extends Sprite {
    constructor(x: number, y: number, numPages?: number);
    getNumPages(): number;
    getCurrentPage(): number;
    setCurrentPage(page: number): void;
    nextPage(): void;
    previousPage(): void;
    pageHasContent(page: number): boolean;
    setPageContent(page: number, hasContent: boolean): void;
    countPagesWithContent(): number;
    delete(): void;
}

/**
 * Bomb class (matches objects.cpp) - Explosive tool
 */
export declare class ToonTalkBomb extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    getFuseTime(): number;
    setFuseTime(time: number): void;
    getTimer(): number;
    setTimer(time: number): void;
    arm(): void;
    explode(): void;
    reset(): void;
    delete(): void;
}

/**
 * Bomb state enum (matches objects.cpp)
 */
export enum BombState {
    INACTIVE = 0,
    ARMED = 1,
    EXPLODING = 2,
    EXPLODED = 3
}

/**
 * Thought_Bubble class (matches objects.cpp) - Robot program container
 */
export declare class ToonTalkThoughtBubble extends Sprite {
    constructor(x: number, y: number);
    hasCubby(): boolean;
    setHasCubby(hasCubby: boolean): void;
    getCubbyId(): number;
    setCubbyId(id: number): void;
    clearCubby(): void;
    delete(): void;
}

/**
 * Mouse class (matches objects.cpp) - Math operator
 */
export declare class ToonTalkMouse extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    getOperationInt(): number;
    setOperationInt(operation: number): void;
    getOperand1(): number;
    setOperand1(value: number): void;
    getOperand2(): number;
    setOperand2(value: number): void;
    getResult(): number;
    doOperation(): void;
    finishOperation(): void;
    delete(): void;
}

/**
 * Mouse state enum (matches objects.cpp)
 */
export enum MouseState {
    IDLE = 0,
    WORKING = 1,
    SMASHING = 2
}

/**
 * Mouse operation enum (matches objects.cpp)
 */
export enum MouseOperation {
    ADD = 0,
    SUBTRACT = 1,
    MULTIPLY = 2,
    DIVIDE = 3,
    NONE = 4
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
