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
    Vacuum: typeof ToonTalkVacuum;
    Martian: typeof ToonTalkMartian;
    Toolbox: typeof ToonTalkToolbox;
    Expander: typeof ToonTalkExpander;
    Copier: typeof ToonTalkCopier;
    Eraser: typeof ToonTalkEraser;
    Cubby: typeof ToonTalkCubby;
    Button: typeof ToonTalkButton;
    Stack: typeof ToonTalkStack;
    Flipper: typeof ToonTalkFlipper;
    Meter: typeof ToonTalkMeter;
    Beeper: typeof ToonTalkBeeper;
    Connector: typeof ToonTalkConnector;
    Timer: typeof ToonTalkTimer;
    Counter: typeof ToonTalkCounter;

    // Enums
    WandMode: typeof WandMode;
    RobotState: typeof RobotState;
    TruckState: typeof TruckState;
    SensorType: typeof SensorType;
    BombState: typeof BombState;
    MouseState: typeof MouseState;
    MouseOperation: typeof MouseOperation;
    VacuumState: typeof VacuumState;
    MartianState: typeof MartianState;
    ToolboxState: typeof ToolboxState;
    ToolboxStack: typeof ToolboxStack;
    ExpanderState: typeof ExpanderState;
    CopierState: typeof CopierState;
    EraserState: typeof EraserState;
    ButtonState: typeof ButtonState;
    FlipperMode: typeof FlipperMode;
    MeterState: typeof MeterState;
    BeeperState: typeof BeeperState;
    ConnectorState: typeof ConnectorState;
    TimerState: typeof TimerState;
    CounterState: typeof CounterState;
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
 * Vacuum class (matches objects.cpp) - Tool that sucks up, blanks, and restores items
 */
export declare class ToonTalkVacuum extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    getSuckedCount(): number;
    setSuckedCount(count: number): void;
    suckUp(): void;
    restore(): void;
    makeBlank(): void;
    delete(): void;
}

/**
 * Vacuum state enum (matches objects.cpp)
 */
export enum VacuumState {
    SUCK = 0,
    SPIT = 1,
    BLANK = 2
}

/**
 * Martian class (matches objects.cpp) - Help character with talk balloon
 */
export declare class ToonTalkMartian extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    hasBalloon(): boolean;
    setHasBalloon(has: boolean): void;
    getMessageId(): number;
    setMessageId(id: number): void;
    isOnFloor(): boolean;
    setOnFloor(on: boolean): void;
    teleportIn(): void;
    teleportOut(): void;
    say(messageId: number): void;
    stopTalking(): void;
    delete(): void;
}

/**
 * Martian state enum (matches objects.cpp)
 */
export enum MartianState {
    IDLE = 0,
    TELEPORTING_IN = 1,
    TELEPORTING_OUT = 2,
    TALKING = 3
}

/**
 * Toolbox class (matches objects.cpp) - Container with stacks of objects
 */
export declare class ToonTalkToolbox extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    getSelectedStack(): number;
    setSelectedStack(stack: number): void;
    getStackCount(stack: number): number;
    setStackCount(stack: number, count: number): void;
    open(): void;
    close(): void;
    finishOpening(): void;
    finishClosing(): void;
    delete(): void;
}

/**
 * Toolbox state enum (matches objects.cpp)
 */
export enum ToolboxState {
    CLOSED = 0,
    OPENING = 1,
    OPEN = 2,
    CLOSING = 3
}

/**
 * Toolbox stack enum (matches objects.cpp)
 */
export enum ToolboxStack {
    NUMBERS = 0,
    TEXTS = 1,
    CUBBIES = 2,
    NESTS = 3,
    SCALES = 4,
    ROBOTS = 5,
    TRUCKS = 6,
    BOMBS = 7,
    STACK_COUNT = 8
}

/**
 * Expander class (matches objects.cpp) - Tool for resizing objects
 */
export declare class ToonTalkExpander extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    getScaleFactor(): number;
    setScaleFactor(scale: number): void;
    getTargetScale(): number;
    setTargetScale(scale: number): void;
    expand(): void;
    shrink(): void;
    setGoodSize(): void;
    delete(): void;
}

/**
 * Expander state enum (matches objects.cpp)
 */
export enum ExpanderState {
    BIGGER = 0,
    TALLER = 1,
    WIDER = 2,
    SMALLER = 3,
    SHORTER = 4,
    NARROWER = 5,
    GOOD_SIZE = 6
}

/**
 * Copier class (matches objects.cpp) - Magic wand for copying objects
 */
export declare class ToonTalkCopier extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    getCopyCount(): number;
    setCopyCount(count: number): void;
    getMaxCopies(): number;
    setMaxCopies(max: number): void;
    hasAttached(): boolean;
    setHasAttached(has: boolean): void;
    makeCopy(): void;
    reset(): void;
    delete(): void;
}

/**
 * Copier state enum (matches objects.cpp)
 */
export enum CopierState {
    NORMAL = 0,
    ORIGINAL = 1,
    SELF = 2
}

/**
 * Eraser class (matches objects.cpp) - Tool for deleting objects
 */
export declare class ToonTalkEraser extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    getErasedCount(): number;
    setErasedCount(count: number): void;
    getEraseProgress(): number;
    setEraseProgress(progress: number): void;
    startErasing(): void;
    erase(): void;
    reset(): void;
    delete(): void;
}

/**
 * Eraser state enum (matches objects.cpp)
 */
export enum EraserState {
    READY = 0,
    ERASING = 1,
    DONE = 2
}

/**
 * Cubby class (matches objects.cpp) - Multi-hole container with labels
 */
export declare class ToonTalkCubby extends Sprite {
    constructor(x: number, y: number, numHoles?: number);
    getNumHoles(): number;
    getFilledCount(): number;
    isHoleFilled(index: number): boolean;
    setHoleFilled(index: number, filled: boolean): void;
    getHoleLabel(index: number): string;
    setHoleLabel(index: number, label: string): void;
    isFull(): boolean;
    isEmpty(): boolean;
    clear(): void;
    delete(): void;
}

/**
 * Button class (matches objects.cpp) - UI control with keyboard binding
 */
export declare class ToonTalkButton extends Sprite {
    constructor(x: number, y: number, keyBinding?: string);
    getStateInt(): number;
    setStateInt(state: number): void;
    getKeyBinding(): string;
    setKeyBinding(key: string): void;
    getPressCount(): number;
    setPressCount(count: number): void;
    isEnabled(): boolean;
    setEnabled(enabled: boolean): void;
    press(): void;
    release(): void;
    hover(): void;
    unhover(): void;
    delete(): void;
}

/**
 * Button state enum (matches objects.cpp)
 */
export enum ButtonState {
    NORMAL = 0,
    HOVERED = 1,
    PRESSED = 2,
    DISABLED = 3
}

/**
 * Stack class (matches objects.cpp) - LIFO data structure
 */
export declare class ToonTalkStack extends Sprite {
    constructor(x: number, y: number, capacity?: number);
    getCapacity(): number;
    getCount(): number;
    isFull(): boolean;
    isEmpty(): boolean;
    getFullness(): number;
    push(): boolean;
    pop(): boolean;
    peek(): number;
    clear(): void;
    setCapacity(capacity: number): void;
    delete(): void;
}

export declare class ToonTalkFlipper extends Sprite {
    constructor(x: number, y: number);
    getModeInt(): number;
    setModeInt(mode: number): void;
    nextMode(): void;
    doFlip(): void;
    reset(): void;
    getFlipCount(): number;
    setFlipCount(count: number): void;
    getRotationAngle(): number;
    setRotationAngle(angle: number): void;
    hasAttached(): boolean;
    setAttached(attached: boolean): void;
    delete(): void;
}

export enum FlipperMode {
    HORIZONTAL = 0,
    VERTICAL = 1,
    ROTATE_CW = 2,
    ROTATE_CCW = 3
}

export declare class ToonTalkMeter extends Sprite {
    constructor(x: number, y: number, minValue?: number, maxValue?: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    getValue(): number;
    setValue(value: number): void;
    getMinValue(): number;
    setMinValue(value: number): void;
    getMaxValue(): number;
    setMaxValue(value: number): void;
    getWarningThreshold(): number;
    setWarningThreshold(value: number): void;
    getCriticalThreshold(): number;
    setCriticalThreshold(value: number): void;
    getPercentage(): number;
    isWarning(): boolean;
    isCritical(): boolean;
    isMaxed(): boolean;
    increment(amount?: number): void;
    decrement(amount?: number): void;
    delete(): void;
}

export enum MeterState {
    NORMAL = 0,
    WARNING = 1,
    CRITICAL = 2,
    MAXED = 3
}

export declare class ToonTalkBeeper extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    beep(): void;
    startContinuous(): void;
    stop(): void;
    getBeepCount(): number;
    setBeepCount(count: number): void;
    getFrequency(): number;
    setFrequency(freq: number): void;
    getDuration(): number;
    setDuration(dur: number): void;
    getTimer(): number;
    isBeeping(): boolean;
    reset(): void;
    delete(): void;
}

export enum BeeperState {
    SILENT = 0,
    BEEPING = 1,
    CONTINUOUS = 2
}

export declare class ToonTalkConnector extends Sprite {
    constructor(x: number, y: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    connect(sourceId: number, targetId: number): void;
    disconnect(): void;
    startConnecting(): void;
    getConnectionCount(): number;
    setConnectionCount(count: number): void;
    getMaxConnections(): number;
    setMaxConnections(max: number): void;
    getSourceId(): number;
    setSourceId(id: number): void;
    getTargetId(): number;
    setTargetId(id: number): void;
    isConnected(): boolean;
    hasError(): boolean;
    delete(): void;
}

export enum ConnectorState {
    DISCONNECTED = 0,
    CONNECTING = 1,
    CONNECTED = 2,
    ERROR = 3
}

export declare class ToonTalkTimer extends Sprite {
    constructor(x: number, y: number, duration?: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    start(): void;
    pause(): void;
    resume(): void;
    stop(): void;
    reset(): void;
    getDuration(): number;
    setDuration(dur: number): void;
    getElapsed(): number;
    setElapsed(elapsed: number): void;
    getRemaining(): number;
    getProgress(): number;
    isLoop(): boolean;
    setLoop(loop: boolean): void;
    getTriggerCount(): number;
    setTriggerCount(count: number): void;
    isRunning(): boolean;
    isFinished(): boolean;
    delete(): void;
}

export enum TimerState {
    STOPPED = 0,
    RUNNING = 1,
    PAUSED = 2,
    FINISHED = 3
}

export declare class ToonTalkCounter extends Sprite {
    constructor(x: number, y: number, initialValue?: number);
    getStateInt(): number;
    setStateInt(state: number): void;
    increment(): void;
    decrement(): void;
    reset(): void;
    getValue(): number;
    setValue(value: number): void;
    getMinValue(): number;
    setMinValue(value: number): void;
    getMaxValue(): number;
    setMaxValue(value: number): void;
    getStep(): number;
    setStep(step: number): void;
    hasMinLimit(): boolean;
    setMinLimit(limit: boolean): void;
    hasMaxLimit(): boolean;
    setMaxLimit(limit: boolean): void;
    isAtMin(): boolean;
    isAtMax(): boolean;
    isOverflow(): boolean;
    delete(): void;
}

export enum CounterState {
    NORMAL = 0,
    AT_MIN = 1,
    AT_MAX = 2,
    OVERFLOW = 3
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
