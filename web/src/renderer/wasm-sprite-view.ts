import * as PIXI from 'pixi.js';
import type { Sprite, Bird, ToonTalkNumber, ToonTalkText, ToonTalkBox, ToonTalkNest, ToonTalkScale, ToonTalkWand, ToonTalkRobot, ToonTalkHouse, ToonTalkTruck, ToonTalkPicture, ToonTalkSensor, ToonTalkNotebook, ToonTalkBomb, ToonTalkThoughtBubble, ToonTalkMouse, ToonTalkVacuum, ToonTalkMartian, ToonTalkToolbox } from '../types/wasm';
import type { ToonTalkRenderer } from './renderer';

/**
 * WasmSpriteView - Bridges WASM Sprite objects with PixiJS rendering
 *
 * This takes a C++ Sprite (or Bird, Number, Text, Box) from WASM and creates a PixiJS
 * visual representation that stays synchronized with the C++ object.
 */
export class WasmSpriteView {
    private wasmSprite: Sprite | Bird | ToonTalkNumber | ToonTalkText | ToonTalkBox | ToonTalkNest | ToonTalkScale | ToonTalkWand | ToonTalkRobot | ToonTalkHouse | ToonTalkTruck | ToonTalkPicture | ToonTalkSensor | ToonTalkNotebook | ToonTalkBomb | ToonTalkThoughtBubble | ToonTalkMouse | ToonTalkVacuum | ToonTalkMartian | ToonTalkToolbox;
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

    constructor(wasmSprite: Sprite | Bird | ToonTalkNumber | ToonTalkText | ToonTalkBox | ToonTalkNest | ToonTalkScale | ToonTalkWand | ToonTalkRobot | ToonTalkHouse | ToonTalkTruck | ToonTalkPicture | ToonTalkSensor | ToonTalkNotebook | ToonTalkBomb | ToonTalkThoughtBubble | ToonTalkMouse | ToonTalkVacuum | ToonTalkMartian | ToonTalkToolbox, stage: PIXI.Container, renderer: ToonTalkRenderer) {
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
        if ('getValue' in this.wasmSprite && !('getTypeInt' in this.wasmSprite) && !('getOperand1' in this.wasmSprite)) return 'number';
        if ('getText' in this.wasmSprite) return 'text';
        if ('getNumHoles' in this.wasmSprite) return 'nest';
        if ('getCapacity' in this.wasmSprite) return 'box';
        if ('getModeInt' in this.wasmSprite && 'nextMode' in this.wasmSprite) return 'wand';
        if ('isActive' in this.wasmSprite && !('getModeInt' in this.wasmSprite) && !('getTypeInt' in this.wasmSprite)) return 'scale';
        if ('getInstructionCount' in this.wasmSprite) return 'robot';
        if ('getNumRooms' in this.wasmSprite) return 'house';
        if ('hasCargo' in this.wasmSprite) return 'truck';
        if ('hasImage' in this.wasmSprite) return 'picture';
        if ('getTypeInt' in this.wasmSprite && 'getValue' in this.wasmSprite) return 'sensor';
        if ('getNumPages' in this.wasmSprite) return 'notebook';
        if ('getFuseTime' in this.wasmSprite) return 'bomb';
        if ('hasCubby' in this.wasmSprite) return 'thoughtbubble';
        if ('getOperand1' in this.wasmSprite && 'doOperation' in this.wasmSprite) return 'mouse';
        if ('getSuckedCount' in this.wasmSprite) return 'vacuum';
        if ('hasBalloon' in this.wasmSprite && 'teleportIn' in this.wasmSprite) return 'martian';
        if ('getSelectedStack' in this.wasmSprite && 'getStackCount' in this.wasmSprite) return 'toolbox';
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
            case 'scale':
                this.drawScale();
                break;
            case 'wand':
                this.drawWand();
                break;
            case 'robot':
                this.drawRobot();
                break;
            case 'house':
                this.drawHouse();
                break;
            case 'truck':
                this.drawTruck();
                break;
            case 'picture':
                this.drawPicture();
                break;
            case 'sensor':
                this.drawSensor();
                break;
            case 'notebook':
                this.drawNotebook();
                break;
            case 'bomb':
                this.drawBomb();
                break;
            case 'thoughtbubble':
                this.drawThoughtBubble();
                break;
            case 'mouse':
                this.drawMouse();
                break;
            case 'vacuum':
                this.drawVacuum();
                break;
            case 'martian':
                this.drawMartian();
                break;
            case 'toolbox':
                this.drawToolbox();
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

    private drawScale(): void {
        const scale = this.wasmSprite as ToonTalkScale;
        const isActive = scale.isActive();

        // Silver/gray scale platform
        this.graphics.beginFill(0xC0C0C0);
        this.graphics.drawRoundedRect(-30, -30, 60, 60, 5);
        this.graphics.endFill();

        // Border (gold if active, gray if not)
        this.graphics.lineStyle(3, isActive ? 0xFFD700 : 0x808080);
        this.graphics.drawRoundedRect(-30, -30, 60, 60, 5);

        // Draw scale platform with balance line
        this.graphics.lineStyle(2, 0x404040);
        this.graphics.moveTo(-25, 0);
        this.graphics.lineTo(25, 0);

        // Draw two pans (left and right)
        this.graphics.beginFill(0xE0E0E0);
        this.graphics.drawCircle(-15, -5, 10);
        this.graphics.drawCircle(15, -5, 10);
        this.graphics.endFill();

        // Label
        const label = new PIXI.Text('Scale', {
            fontSize: 10,
            fill: isActive ? 0xFFD700 : 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 20;
        this.graphics.addChild(label);

        // Status text
        const statusText = new PIXI.Text(isActive ? 'ACTIVE' : 'Ready', {
            fontSize: 8,
            fill: isActive ? 0xFFD700 : 0xCCCCCC,
            stroke: 0x000000,
            strokeThickness: 1
        });
        statusText.anchor.set(0.5);
        statusText.y = -22;
        this.graphics.addChild(statusText);
    }

    private drawWand(): void {
        const wand = this.wasmSprite as ToonTalkWand;
        const mode = wand.getModeInt();
        const isActive = wand.isActive();

        // Wand stick (brown handle)
        this.graphics.beginFill(0x8B4513);
        this.graphics.drawRect(-5, -20, 10, 40);
        this.graphics.endFill();

        // Star at top (color changes based on mode)
        const modeColors = [
            0x32CD32, // CREATE_NUMBER - green
            0xFFFF00, // CREATE_TEXT - yellow
            0x8B4513, // CREATE_BOX - brown
            0x9370DB, // CREATE_NEST - purple
            0xFFD700  // CREATE_BIRD - gold
        ];
        const starColor = modeColors[mode] || 0xFFFFFF;

        // Draw star
        this.graphics.beginFill(starColor);
        const numPoints = 5;
        const outerRadius = 15;
        const innerRadius = 7;
        for (let i = 0; i < numPoints * 2; i++) {
            const radius = i % 2 === 0 ? outerRadius : innerRadius;
            const angle = (i * Math.PI) / numPoints - Math.PI / 2;
            const x = Math.cos(angle) * radius;
            const y = Math.sin(angle) * radius - 30;
            if (i === 0) {
                this.graphics.moveTo(x, y);
            } else {
                this.graphics.lineTo(x, y);
            }
        }
        this.graphics.closePath();
        this.graphics.endFill();

        // Border/glow if active
        if (isActive) {
            this.graphics.lineStyle(3, 0xFFFFFF, 0.8);
            for (let i = 0; i < numPoints * 2; i++) {
                const radius = i % 2 === 0 ? outerRadius : innerRadius;
                const angle = (i * Math.PI) / numPoints - Math.PI / 2;
                const x = Math.cos(angle) * radius;
                const y = Math.sin(angle) * radius - 30;
                if (i === 0) {
                    this.graphics.moveTo(x, y);
                } else {
                    this.graphics.lineTo(x, y);
                }
            }
            this.graphics.closePath();
        }

        // Label
        const modeNames = ['Number', 'Text', 'Box', 'Nest', 'Bird'];
        const label = new PIXI.Text('Wand', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 25;
        this.graphics.addChild(label);

        // Mode text
        const modeText = new PIXI.Text(modeNames[mode] || '?', {
            fontSize: 8,
            fill: starColor,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        modeText.anchor.set(0.5);
        modeText.y = 15;
        this.graphics.addChild(modeText);
    }

    private drawRobot(): void {
        const robot = this.wasmSprite as ToonTalkRobot;
        const state = robot.getStateInt();
        const instructions = robot.getInstructionCount();

        // Robot body (metallic gray)
        this.graphics.beginFill(0x708090);
        this.graphics.drawRoundedRect(-35, -45, 70, 90, 5);
        this.graphics.endFill();

        // Border color changes with state
        const stateColors = [
            0xCCCCCC, // IDLE - gray
            0x00FF00, // RUNNING - green
            0xFFFF00, // PAUSED - yellow
            0xFF00FF  // TRAINING - magenta
        ];
        this.graphics.lineStyle(3, stateColors[state] || 0xCCCCCC);
        this.graphics.drawRoundedRect(-35, -45, 70, 90, 5);

        // Robot head (rounded top)
        this.graphics.beginFill(0x909090);
        this.graphics.drawCircle(0, -35, 20);
        this.graphics.endFill();

        // Eyes (two small circles)
        this.graphics.beginFill(state === 1 ? 0x00FF00 : 0x4169E1); // Green when running
        this.graphics.drawCircle(-8, -38, 5);
        this.graphics.drawCircle(8, -38, 5);
        this.graphics.endFill();

        // Antenna
        this.graphics.lineStyle(2, 0xFFD700);
        this.graphics.moveTo(0, -55);
        this.graphics.lineTo(0, -65);
        this.graphics.beginFill(0xFFD700);
        this.graphics.drawCircle(0, -65, 3);
        this.graphics.endFill();

        // Label
        const label = new PIXI.Text('Robot', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 30;
        this.graphics.addChild(label);

        // State text
        const stateNames = ['IDLE', 'RUNNING', 'PAUSED', 'TRAINING'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: stateColors[state] || 0xCCCCCC,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -20;
        this.graphics.addChild(stateText);

        // Instruction count
        const instText = new PIXI.Text(`${instructions} inst`, {
            fontSize: 8,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 1
        });
        instText.anchor.set(0.5);
        instText.y = 15;
        this.graphics.addChild(instText);
    }

    private drawHouse(): void {
        const house = this.wasmSprite as ToonTalkHouse;
        const numRooms = house.getNumRooms();
        const occupiedRooms = house.countOccupiedRooms();

        // House body (brown)
        this.graphics.beginFill(0xD2691E);
        this.graphics.drawRect(-75, -40, 150, 80);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x8B4513);
        this.graphics.drawRect(-75, -40, 150, 80);

        // Roof (triangle - red)
        this.graphics.beginFill(0xDC143C);
        this.graphics.moveTo(-80, -40);
        this.graphics.lineTo(0, -70);
        this.graphics.lineTo(80, -40);
        this.graphics.lineTo(-80, -40);
        this.graphics.endFill();

        // Roof border
        this.graphics.lineStyle(2, 0x8B0000);
        this.graphics.moveTo(-80, -40);
        this.graphics.lineTo(0, -70);
        this.graphics.lineTo(80, -40);

        // Windows (rooms)
        const windowSpacing = 140 / (numRooms + 1);
        for (let i = 0; i < numRooms; i++) {
            const windowX = -70 + (i + 1) * windowSpacing;
            const isOccupied = house.isRoomOccupied(i);

            // Window
            this.graphics.beginFill(isOccupied ? 0xFFFF00 : 0x87CEEB);
            this.graphics.drawRect(windowX - 10, -20, 20, 25);
            this.graphics.endFill();

            // Window pane
            this.graphics.lineStyle(2, 0x654321);
            this.graphics.drawRect(windowX - 10, -20, 20, 25);
            this.graphics.moveTo(windowX, -20);
            this.graphics.lineTo(windowX, 5);
            this.graphics.moveTo(windowX - 10, -7.5);
            this.graphics.lineTo(windowX + 10, -7.5);
        }

        // Door
        this.graphics.beginFill(0x654321);
        this.graphics.drawRect(-12, 10, 24, 30);
        this.graphics.endFill();

        // Doorknob
        this.graphics.beginFill(0xFFD700);
        this.graphics.drawCircle(8, 25, 2);
        this.graphics.endFill();

        // Label
        const label = new PIXI.Text('House', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 50;
        this.graphics.addChild(label);

        // Room status
        const statusText = new PIXI.Text(`${occupiedRooms}/${numRooms} rooms`, {
            fontSize: 8,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 2
        });
        statusText.anchor.set(0.5);
        statusText.y = -55;
        this.graphics.addChild(statusText);
    }

    private drawTruck(): void {
        const truck = this.wasmSprite as ToonTalkTruck;
        const state = truck.getStateInt();
        const hasCargo = truck.hasCargo();

        // Truck bed (cargo area - red)
        this.graphics.beginFill(hasCargo ? 0xFF4500 : 0xDC143C);
        this.graphics.drawRect(-40, -20, 50, 35);
        this.graphics.endFill();

        // Cargo area border
        this.graphics.lineStyle(2, 0x8B0000);
        this.graphics.drawRect(-40, -20, 50, 35);

        // Truck cab (front - darker red)
        this.graphics.beginFill(0xB22222);
        this.graphics.drawRect(10, -15, 30, 30);
        this.graphics.endFill();

        // Cab border
        this.graphics.lineStyle(2, 0x8B0000);
        this.graphics.drawRect(10, -15, 30, 30);

        // Windshield
        this.graphics.beginFill(0x87CEEB);
        this.graphics.drawRect(15, -10, 20, 12);
        this.graphics.endFill();

        // Wheels (black circles)
        this.graphics.beginFill(0x000000);
        this.graphics.drawCircle(-25, 20, 8);
        this.graphics.drawCircle(15, 20, 8);
        this.graphics.endFill();

        // Wheel rims
        this.graphics.lineStyle(2, 0x808080);
        this.graphics.drawCircle(-25, 20, 5);
        this.graphics.drawCircle(15, 20, 5);

        // Cargo indicator if loaded
        if (hasCargo) {
            this.graphics.beginFill(0xFFD700);
            this.graphics.drawRect(-35, -15, 40, 25);
            this.graphics.endFill();

            const cargoLabel = new PIXI.Text('📦', {
                fontSize: 16
            });
            cargoLabel.anchor.set(0.5);
            cargoLabel.position.set(-15, -2);
            this.graphics.addChild(cargoLabel);
        }

        // Label
        const label = new PIXI.Text('Truck', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 35;
        this.graphics.addChild(label);

        // State indicator
        const stateNames = ['EMPTY', 'LOADED', 'DELIVERING'];
        const stateColors = [0xCCCCCC, 0xFFD700, 0x00FF00];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: stateColors[state] || 0xCCCCCC,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -30;
        this.graphics.addChild(stateText);
    }

    private drawPicture(): void {
        const picture = this.wasmSprite as ToonTalkPicture;
        const hasImage = picture.hasImage();
        const width = picture.getPictureWidth();
        const height = picture.getPictureHeight();

        // Picture frame (golden/wood color)
        this.graphics.beginFill(0xDAA520);
        this.graphics.drawRect(-width/2 - 5, -height/2 - 5, width + 10, height + 10);
        this.graphics.endFill();

        // Picture canvas area
        if (hasImage) {
            // Show gradient for image placeholder
            this.graphics.beginFill(0x87CEEB);
            this.graphics.drawRect(-width/2, -height/2, width, height);
            this.graphics.endFill();

            // Image ID display
            const imageText = new PIXI.Text(`IMG #${picture.getImageId()}`, {
                fontSize: 14,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2
            });
            imageText.anchor.set(0.5);
            imageText.y = 0;
            this.graphics.addChild(imageText);

            // Picture icon
            const iconText = new PIXI.Text('🖼️', {
                fontSize: 24
            });
            iconText.anchor.set(0.5);
            iconText.y = -20;
            this.graphics.addChild(iconText);
        } else {
            // Empty canvas - white
            this.graphics.beginFill(0xF5F5F5);
            this.graphics.drawRect(-width/2, -height/2, width, height);
            this.graphics.endFill();

            // X pattern for empty
            this.graphics.lineStyle(2, 0xCCCCCC);
            this.graphics.moveTo(-width/2, -height/2);
            this.graphics.lineTo(width/2, height/2);
            this.graphics.moveTo(width/2, -height/2);
            this.graphics.lineTo(-width/2, height/2);

            const emptyText = new PIXI.Text('Empty', {
                fontSize: 12,
                fill: 0x888888
            });
            emptyText.anchor.set(0.5);
            this.graphics.addChild(emptyText);
        }

        // Label
        const label = new PIXI.Text('Picture', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = height/2 + 15;
        this.graphics.addChild(label);
    }

    private drawSensor(): void {
        const sensor = this.wasmSprite as ToonTalkSensor;
        const type = sensor.getTypeInt();
        const isActive = sensor.isActive();
        const value = sensor.getValue();

        // Sensor body (dark gray circuit board)
        this.graphics.beginFill(0x2F4F4F);
        this.graphics.drawRoundedRect(-30, -30, 60, 60, 5);
        this.graphics.endFill();

        // Border glows when active
        this.graphics.lineStyle(3, isActive ? 0x00FF00 : 0x696969);
        this.graphics.drawRoundedRect(-30, -30, 60, 60, 5);

        // Sensor icon based on type
        const typeIcons = ['🖱️', '⌨️', '⏰', '💥']; // MOUSE, KEYBOARD, TIME, COLLISION
        const typeNames = ['Mouse', 'Keyboard', 'Time', 'Collision'];
        const icon = new PIXI.Text(typeIcons[type] || '❓', {
            fontSize: 24
        });
        icon.anchor.set(0.5);
        icon.y = -8;
        this.graphics.addChild(icon);

        // LED indicator
        this.graphics.beginFill(isActive ? 0x00FF00 : 0x444444);
        this.graphics.drawCircle(15, -20, 4);
        this.graphics.endFill();

        // Label
        const label = new PIXI.Text('Sensor', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 22;
        this.graphics.addChild(label);

        // Type label
        const typeLabel = new PIXI.Text(typeNames[type] || '?', {
            fontSize: 8,
            fill: isActive ? 0x00FF00 : 0xCCCCCC,
            stroke: 0x000000,
            strokeThickness: 1
        });
        typeLabel.anchor.set(0.5);
        typeLabel.y = 12;
        this.graphics.addChild(typeLabel);

        // Value display (if active)
        if (isActive && value !== 0) {
            const valueText = new PIXI.Text(`${value.toFixed(1)}`, {
                fontSize: 8,
                fill: 0x00FF00,
                stroke: 0x000000,
                strokeThickness: 1
            });
            valueText.anchor.set(0.5);
            valueText.y = -22;
            this.graphics.addChild(valueText);
        }
    }

    private drawNotebook(): void {
        const notebook = this.wasmSprite as ToonTalkNotebook;
        const numPages = notebook.getNumPages();
        const currentPage = notebook.getCurrentPage();
        const pagesWithContent = notebook.countPagesWithContent();

        // Notebook cover (brown leather)
        this.graphics.beginFill(0x8B4513);
        this.graphics.drawRoundedRect(-60, -50, 120, 100, 5);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x654321);
        this.graphics.drawRoundedRect(-60, -50, 120, 100, 5);

        // Spine decoration
        this.graphics.lineStyle(2, 0x654321);
        this.graphics.moveTo(-55, -45);
        this.graphics.lineTo(-55, 45);

        // Pages (white edge)
        this.graphics.beginFill(0xFFFFF0);
        this.graphics.drawRect(50, -45, 8, 90);
        this.graphics.endFill();

        // Page lines
        this.graphics.lineStyle(1, 0xE0E0E0);
        for (let i = 0; i < 5; i++) {
            const y = -30 + i * 15;
            this.graphics.moveTo(50, y);
            this.graphics.lineTo(58, y);
        }

        // Notebook icon
        const icon = new PIXI.Text('📓', {
            fontSize: 32
        });
        icon.anchor.set(0.5);
        icon.position.set(-8, -10);
        this.graphics.addChild(icon);

        // Label
        const label = new PIXI.Text('Notebook', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 35;
        this.graphics.addChild(label);

        // Page counter
        const pageText = new PIXI.Text(`Page ${currentPage + 1}/${numPages}`, {
            fontSize: 8,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 2
        });
        pageText.anchor.set(0.5);
        pageText.y = 22;
        this.graphics.addChild(pageText);

        // Content indicator
        const contentText = new PIXI.Text(`${pagesWithContent} with content`, {
            fontSize: 7,
            fill: 0xFFD700,
            stroke: 0x000000,
            strokeThickness: 1
        });
        contentText.anchor.set(0.5);
        contentText.y = -40;
        this.graphics.addChild(contentText);
    }

    private drawBomb(): void {
        const bomb = this.wasmSprite as ToonTalkBomb;
        const state = bomb.getStateInt();
        const timer = bomb.getTimer();
        const fuseTime = bomb.getFuseTime();

        // Bomb body (black/dark gray sphere)
        this.graphics.beginFill(state === 2 ? 0xFF4500 : 0x2F2F2F); // Orange-red when exploding
        this.graphics.drawCircle(0, 0, 25);
        this.graphics.endFill();

        // Border changes based on state
        const stateColors = [
            0x505050, // INACTIVE - gray
            0xFF0000, // ARMED - red
            0xFF8C00, // EXPLODING - orange
            0x808080  // EXPLODED - gray
        ];
        this.graphics.lineStyle(3, stateColors[state] || 0x505050);
        this.graphics.drawCircle(0, 0, 25);

        // Fuse (curvy line to top)
        if (state !== 3) { // Not exploded
            this.graphics.lineStyle(2, 0x8B4513);
            this.graphics.moveTo(0, -25);
            this.graphics.bezierCurveTo(-5, -35, 5, -35, 0, -40);
        }

        // Spark at fuse end (if armed or exploding)
        if (state === 1 || state === 2) {
            const sparkSize = state === 2 ? 8 : 4;
            this.graphics.beginFill(state === 2 ? 0xFFFF00 : 0xFFD700);
            this.drawStar(0, -40, 4, sparkSize, sparkSize * 0.5);
            this.graphics.endFill();
        }

        // Explosion effect (if exploding)
        if (state === 2) {
            for (let i = 0; i < 8; i++) {
                const angle = (i / 8) * Math.PI * 2;
                const distance = 35;
                const x = Math.cos(angle) * distance;
                const y = Math.sin(angle) * distance;
                this.graphics.beginFill(0xFF4500, 0.7);
                this.drawStar(x, y, 4, 8, 4);
                this.graphics.endFill();
            }
        }

        // Label
        const label = new PIXI.Text('Bomb', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 40;
        this.graphics.addChild(label);

        // State text
        const stateNames = ['INACTIVE', 'ARMED', 'EXPLODING!', 'EXPLODED'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: stateColors[state] || 0xCCCCCC,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -28;
        this.graphics.addChild(stateText);

        // Timer display (if armed)
        if (state === 1) {
            const remaining = Math.max(0, fuseTime - timer);
            const timerText = new PIXI.Text(`${remaining.toFixed(1)}s`, {
                fontSize: 12,
                fill: remaining < 1 ? 0xFF0000 : 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2
            });
            timerText.anchor.set(0.5);
            timerText.y = 0;
            this.graphics.addChild(timerText);
        }

        // Explosion icon
        if (state === 2 || state === 3) {
            const icon = new PIXI.Text('💥', {
                fontSize: 24
            });
            icon.anchor.set(0.5);
            icon.y = 0;
            this.graphics.addChild(icon);
        }
    }

    private drawThoughtBubble(): void {
        const bubble = this.wasmSprite as ToonTalkThoughtBubble;
        const hasCubby = bubble.hasCubby();
        const cubbyId = bubble.getCubbyId();

        // Main thought bubble (white cloud)
        this.graphics.beginFill(0xFFFFFF, 0.9);
        this.graphics.drawEllipse(0, -10, 50, 40);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(2, 0x888888);
        this.graphics.drawEllipse(0, -10, 50, 40);

        // Small thought bubbles (descending)
        this.graphics.beginFill(0xFFFFFF, 0.9);
        this.graphics.drawCircle(-25, 20, 10);
        this.graphics.drawCircle(-35, 30, 6);
        this.graphics.endFill();

        this.graphics.lineStyle(2, 0x888888);
        this.graphics.drawCircle(-25, 20, 10);
        this.graphics.drawCircle(-35, 30, 6);

        // Content indicator
        if (hasCubby) {
            // Show robot program icon
            const icon = new PIXI.Text('🤖', {
                fontSize: 24
            });
            icon.anchor.set(0.5);
            icon.position.set(0, -15);
            this.graphics.addChild(icon);

            // Cubby ID
            const idText = new PIXI.Text(`#${cubbyId}`, {
                fontSize: 10,
                fill: 0x0000FF,
                fontWeight: 'bold',
                stroke: 0xFFFFFF,
                strokeThickness: 2
            });
            idText.anchor.set(0.5);
            idText.y = 5;
            this.graphics.addChild(idText);
        } else {
            // Empty thought
            const emptyText = new PIXI.Text('...', {
                fontSize: 20,
                fill: 0x888888
            });
            emptyText.anchor.set(0.5);
            emptyText.y = -15;
            this.graphics.addChild(emptyText);
        }

        // Label
        const label = new PIXI.Text('Thought', {
            fontSize: 10,
            fill: 0x000000,
            fontWeight: 'bold',
            stroke: 0xFFFFFF,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 45;
        this.graphics.addChild(label);
    }

    private drawMouse(): void {
        const mouse = this.wasmSprite as ToonTalkMouse;
        const state = mouse.getStateInt();
        const operation = mouse.getOperationInt();
        const result = mouse.getResult();

        // Mouse body (gray, rounded)
        this.graphics.beginFill(0x808080);
        this.graphics.drawEllipse(0, 0, 30, 25);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(2, 0x505050);
        this.graphics.drawEllipse(0, 0, 30, 25);

        // Ears
        this.graphics.beginFill(0x909090);
        this.graphics.drawCircle(-20, -15, 12);
        this.graphics.drawCircle(20, -15, 12);
        this.graphics.endFill();

        // Eyes
        const eyeColor = state === 1 ? 0xFFFF00 : (state === 2 ? 0xFF0000 : 0x000000);
        this.graphics.beginFill(eyeColor);
        this.graphics.drawCircle(-10, -5, 4);
        this.graphics.drawCircle(10, -5, 4);
        this.graphics.endFill();

        // Nose
        this.graphics.beginFill(0xFF69B4);
        this.graphics.drawCircle(0, 5, 3);
        this.graphics.endFill();

        // Tail (curvy)
        this.graphics.lineStyle(2, 0x505050);
        this.graphics.moveTo(28, 0);
        this.graphics.bezierCurveTo(40, -10, 45, 10, 50, 0);

        // Hammer (if working or smashing)
        if (state === 1 || state === 2) {
            const hammerY = state === 2 ? 15 : -5; // Raised when smashing
            // Handle
            this.graphics.beginFill(0x8B4513);
            this.graphics.drawRect(-5, hammerY, 3, 20);
            this.graphics.endFill();

            // Head
            this.graphics.beginFill(0x696969);
            this.graphics.drawRect(-10, hammerY, 13, 8);
            this.graphics.endFill();
        }

        // Label
        const label = new PIXI.Text('Mouse', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 35;
        this.graphics.addChild(label);

        // Operation symbol
        const opSymbols = ['+', '-', '×', '÷', ''];
        if (operation < 4) {
            const opText = new PIXI.Text(opSymbols[operation], {
                fontSize: 16,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2
            });
            opText.anchor.set(0.5);
            opText.y = -25;
            this.graphics.addChild(opText);
        }

        // Result (if smashing)
        if (state === 2 && result !== 0) {
            const resultText = new PIXI.Text(`=${result.toFixed(1)}`, {
                fontSize: 10,
                fill: 0x00FF00,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2
            });
            resultText.anchor.set(0.5);
            resultText.y = 25;
            this.graphics.addChild(resultText);
        }

        // State indicator
        const stateNames = ['IDLE', 'WORKING', 'SMASHING'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 7,
            fill: state === 0 ? 0xCCCCCC : (state === 1 ? 0xFFFF00 : 0xFF0000),
            stroke: 0x000000,
            strokeThickness: 1
        });
        stateText.anchor.set(0.5);
        stateText.y = 45;
        this.graphics.addChild(stateText);
    }

    /**
     * Helper method to draw a star shape using PixiJS Graphics
     * @param x Center X position
     * @param y Center Y position
     * @param points Number of star points
     * @param outerRadius Outer radius of star
     * @param innerRadius Inner radius of star
     */
    private drawStar(x: number, y: number, points: number, outerRadius: number, innerRadius: number): void {
        const path: number[] = [];
        const step = Math.PI / points;

        for (let i = 0; i < points * 2; i++) {
            const radius = i % 2 === 0 ? outerRadius : innerRadius;
            const angle = i * step - Math.PI / 2;
            path.push(x + Math.cos(angle) * radius);
            path.push(y + Math.sin(angle) * radius);
        }

        this.graphics.drawPolygon(path);
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

    private drawVacuum(): void {
        const vacuum = this.wasmSprite as ToonTalkVacuum;
        const state = vacuum.getStateInt();
        const suckedCount = vacuum.getSuckedCount();

        // Vacuum body (cylindrical vacuum cleaner)
        const bodyColor = state === 0 ? 0x4169E1 : (state === 1 ? 0xFF6347 : 0x9370DB); // Blue=SUCK, Red=SPIT, Purple=BLANK
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-25, -30, 50, 60, 5);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(2, 0x000000);
        this.graphics.drawRoundedRect(-25, -30, 50, 60, 5);

        // Nozzle/hose
        this.graphics.lineStyle(6, 0x696969);
        if (state === 0) { // SUCK - hose pointing down
            this.graphics.moveTo(0, 30);
            this.graphics.lineTo(0, 40);
            this.graphics.lineTo(-10, 45);
        } else if (state === 1) { // SPIT - hose pointing up
            this.graphics.moveTo(0, -30);
            this.graphics.lineTo(0, -40);
            this.graphics.lineTo(10, -45);
        }

        // Suction/emission effect
        if (state === 0 || state === 1) {
            const dir = state === 0 ? 1 : -1;
            for (let i = 0; i < 3; i++) {
                const alpha = 0.3 - i * 0.1;
                this.graphics.lineStyle(2, 0xFFFFFF, alpha);
                this.graphics.drawCircle(0, dir * (35 + i * 5), 8 + i * 4);
            }
        }

        // Counter display
        if (suckedCount > 0) {
            const counterText = new PIXI.Text(`${suckedCount}`, {
                fontSize: 16,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 3
            });
            counterText.anchor.set(0.5);
            counterText.y = 0;
            this.graphics.addChild(counterText);
        }

        // Label
        const label = new PIXI.Text('Vacuum', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 45;
        this.graphics.addChild(label);

        // State text
        const stateNames = ['SUCK', 'SPIT', 'BLANK'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: 0xFFFF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -40;
        this.graphics.addChild(stateText);
    }

    private drawMartian(): void {
        const martian = this.wasmSprite as ToonTalkMartian;
        const state = martian.getStateInt();
        const hasBalloon = martian.hasBalloon();
        const messageId = martian.getMessageId();

        // Martian body (alien character)
        const bodyColor = state === 3 ? 0x00FF00 : 0x7FFF00; // Bright green when talking
        const alpha = (state === 1 || state === 2) ? 0.5 : 1.0; // Semi-transparent when teleporting

        this.graphics.beginFill(bodyColor, alpha);
        this.graphics.drawEllipse(0, 0, 30, 35); // Body
        this.graphics.endFill();

        // Head (top circle)
        this.graphics.beginFill(bodyColor, alpha);
        this.graphics.drawCircle(0, -25, 20);
        this.graphics.endFill();

        // Antennae
        if (alpha > 0.7) {
            this.graphics.lineStyle(2, 0x228B22);
            this.graphics.moveTo(-10, -40);
            this.graphics.lineTo(-15, -50);
            this.graphics.moveTo(10, -40);
            this.graphics.lineTo(15, -50);

            // Antenna balls
            this.graphics.beginFill(0xFF0000);
            this.graphics.drawCircle(-15, -50, 3);
            this.graphics.drawCircle(15, -50, 3);
            this.graphics.endFill();
        }

        // Eyes
        this.graphics.beginFill(0xFFFFFF);
        this.graphics.drawCircle(-8, -25, 6);
        this.graphics.drawCircle(8, -25, 6);
        this.graphics.endFill();

        // Pupils
        this.graphics.beginFill(0x000000);
        this.graphics.drawCircle(-8, -25, 3);
        this.graphics.drawCircle(8, -25, 3);
        this.graphics.endFill();

        // Teleport effect
        if (state === 1 || state === 2) {
            for (let i = 0; i < 5; i++) {
                const radius = 40 + i * 8;
                const sparkleAlpha = 0.4 - i * 0.08;
                this.graphics.lineStyle(2, 0x00FFFF, sparkleAlpha);
                this.graphics.drawCircle(0, 0, radius);
            }
        }

        // Talk balloon
        if (hasBalloon && state === 3) {
            const balloon = new PIXI.Text(`💬 #${messageId}`, {
                fontSize: 14,
                fill: 0xFFFFFF,
                backgroundColor: 0x000000,
                padding: 4
            });
            balloon.anchor.set(0.5);
            balloon.x = 40;
            balloon.y = -30;
            this.graphics.addChild(balloon);
        }

        // Label
        const label = new PIXI.Text('Martian', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 50;
        this.graphics.addChild(label);

        // State text
        const stateNames = ['IDLE', 'IN', 'OUT', 'TALKING'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: 0x00FFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = 60;
        this.graphics.addChild(stateText);
    }

    private drawToolbox(): void {
        const toolbox = this.wasmSprite as ToonTalkToolbox;
        const state = toolbox.getStateInt();
        const selectedStack = toolbox.getSelectedStack();

        // Toolbox body (large rectangular container)
        const isOpen = state === 2; // OPEN
        const height = isOpen ? 70 : 50;

        this.graphics.beginFill(0x8B4513); // Brown toolbox
        this.graphics.drawRoundedRect(-75, -height / 2, 150, height, 8);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x000000);
        this.graphics.drawRoundedRect(-75, -height / 2, 150, height, 8);

        // Handle
        if (!isOpen) {
            this.graphics.lineStyle(4, 0xFFD700);
            this.graphics.moveTo(-20, -25);
            this.graphics.bezierCurveTo(-20, -35, 20, -35, 20, -25);
        }

        // If open, show compartments with stack icons
        if (isOpen) {
            const stackNames = ['123', 'ABC', '□', '⊡', '⚖', '🤖', '🚚', '💣'];
            const compartmentWidth = 30;
            const startX = -60;

            for (let i = 0; i < 5; i++) {
                const x = startX + i * compartmentWidth;
                const isSelected = i === selectedStack;

                // Compartment background
                this.graphics.beginFill(isSelected ? 0xFFD700 : 0xD2691E);
                this.graphics.drawRect(x, -15, 28, 25);
                this.graphics.endFill();

                // Stack icon/label
                const icon = new PIXI.Text(stackNames[i] || '?', {
                    fontSize: 12,
                    fill: isSelected ? 0x000000 : 0xFFFFFF,
                    fontWeight: 'bold'
                });
                icon.anchor.set(0.5);
                icon.x = x + 14;
                icon.y = -2;
                this.graphics.addChild(icon);

                // Stack count
                const count = toolbox.getStackCount(i);
                const countText = new PIXI.Text(`${count}`, {
                    fontSize: 8,
                    fill: isSelected ? 0x8B0000 : 0xFFFFFF,
                    fontWeight: 'bold'
                });
                countText.anchor.set(0.5);
                countText.x = x + 14;
                countText.y = 8;
                this.graphics.addChild(countText);
            }
        }

        // Label
        const label = new PIXI.Text('Toolbox', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = height / 2 + 10;
        this.graphics.addChild(label);

        // State text
        const stateNames = ['CLOSED', 'OPENING', 'OPEN', 'CLOSING'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: 0xFFD700,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -height / 2 - 10;
        this.graphics.addChild(stateText);
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
        } else if (type === 'nest' && this.textDisplay) {
            const nest = this.wasmSprite as ToonTalkNest;
            this.textDisplay.text = `${nest.countOccupied()} / ${nest.getNumHoles()}`;

            // Also update the hole visuals
            this.drawNest(); // Redraw to show filled/empty holes
        }
    }

    /**
     * Get the underlying WASM sprite
     */
    getWasmSprite(): Sprite | Bird | ToonTalkNumber | ToonTalkText | ToonTalkBox | ToonTalkNest {
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

        // Nest can accept any object (into holes)
        if (myType === 'nest') return true;

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

        // Nest accepts anything (into first empty hole)
        else if (myType === 'nest') {
            const nest = this.wasmSprite as ToonTalkNest;

            if (!nest.isFull()) {
                // Find first empty hole
                const numHoles = nest.getNumHoles();
                for (let i = 0; i < numHoles; i++) {
                    if (nest.isHoleEmpty(i)) {
                        nest.setHole(i, true);
                        console.log(`✨ Added item to nest hole ${i} (${nest.countOccupied()} / ${numHoles})`);

                        // Remove the dropped object
                        this.renderer.removeWasmSprite(droppedSprite);
                        break;
                    }
                }
            } else {
                console.log('❌ Nest is full!');
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
