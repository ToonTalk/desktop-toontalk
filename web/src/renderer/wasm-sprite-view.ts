import * as PIXI from 'pixi.js';
import type { Sprite, Bird, ToonTalkNumber, ToonTalkText, ToonTalkBox, ToonTalkNest, ToonTalkScale, ToonTalkWand, ToonTalkRobot, ToonTalkHouse, ToonTalkTruck, ToonTalkPicture, ToonTalkSensor, ToonTalkNotebook, ToonTalkBomb, ToonTalkThoughtBubble, ToonTalkMouse, ToonTalkVacuum, ToonTalkMartian, ToonTalkToolbox, ToonTalkExpander, ToonTalkCopier, ToonTalkEraser, ToonTalkCubby, ToonTalkButton, ToonTalkStack, ToonTalkFlipper, ToonTalkMeter, ToonTalkBeeper, ToonTalkConnector, ToonTalkTimer, ToonTalkCounter, ToonTalkSampler, ToonTalkComparator, ToonTalkRandomizer, ToonTalkLogger, ToonTalkFilter, ToonTalkAccumulator, ToonTalkSequencer, ToonTalkTrigger, ToonTalkScheduler } from '../types/wasm';
import type { ToonTalkRenderer } from './renderer';
import { getGameEngine } from '../core/game-engine';
import { getTextureManager } from '../core/texture-manager';

/**
 * WasmSpriteView - Bridges WASM Sprite objects with PixiJS rendering
 *
 * This takes a C++ Sprite (or Bird, Number, Text, Box) from WASM and creates a PixiJS
 * visual representation that stays synchronized with the C++ object.
 *
 * Now enhanced with WASM game engine utilities:
 * - Collision detection via CollisionManager
 * - Smooth animations via AnimationController
 * - Entity tracking via EntityManager
 */
export class WasmSpriteView {
    private wasmSprite: Sprite | Bird | ToonTalkNumber | ToonTalkText | ToonTalkBox | ToonTalkNest | ToonTalkScale | ToonTalkWand | ToonTalkRobot | ToonTalkHouse | ToonTalkTruck | ToonTalkPicture | ToonTalkSensor | ToonTalkNotebook | ToonTalkBomb | ToonTalkThoughtBubble | ToonTalkMouse | ToonTalkVacuum | ToonTalkMartian | ToonTalkToolbox | ToonTalkExpander | ToonTalkCopier | ToonTalkEraser | ToonTalkCubby | ToonTalkButton | ToonTalkStack | ToonTalkFlipper | ToonTalkMeter | ToonTalkBeeper | ToonTalkConnector | ToonTalkTimer | ToonTalkCounter | ToonTalkSampler | ToonTalkComparator | ToonTalkRandomizer | ToonTalkLogger | ToonTalkFilter | ToonTalkAccumulator | ToonTalkSequencer | ToonTalkTrigger | ToonTalkScheduler;
    private container: PIXI.Container;  // Container for both sprite and graphics
    private sprite?: PIXI.Sprite;       // Textured sprite (if texture available)
    private graphics: PIXI.Graphics;    // Graphics overlay (for text, borders, etc.)
    private textDisplay?: PIXI.Text | PIXI.Container;
    private destroyed: boolean = false;
    private renderer: ToonTalkRenderer;

    // Drag state
    private isDragging: boolean = false;
    private dragOffset: { x: number; y: number } = { x: 0, y: 0 };
    private originalScale: { x: number; y: number } = { x: 1, y: 1 };
    private isHovered: boolean = false;
    private dropTarget: WasmSpriteView | null = null;
    private dropOnLeftHalf: boolean = true; // Track which half we're dropping on

    // Collision & Animation IDs
    private collisionId: number = -1;
    private entityId: number = -1;
    private dropAnimationId: number = -1;

    constructor(wasmSprite: Sprite | Bird | ToonTalkNumber | ToonTalkText | ToonTalkBox | ToonTalkNest | ToonTalkScale | ToonTalkWand | ToonTalkRobot | ToonTalkHouse | ToonTalkTruck | ToonTalkPicture | ToonTalkSensor | ToonTalkNotebook | ToonTalkBomb | ToonTalkThoughtBubble | ToonTalkMouse | ToonTalkVacuum | ToonTalkMartian | ToonTalkToolbox | ToonTalkExpander | ToonTalkCopier | ToonTalkEraser | ToonTalkCubby | ToonTalkButton | ToonTalkStack | ToonTalkFlipper | ToonTalkMeter | ToonTalkBeeper | ToonTalkConnector | ToonTalkTimer | ToonTalkCounter | ToonTalkSampler | ToonTalkComparator | ToonTalkRandomizer | ToonTalkLogger | ToonTalkFilter | ToonTalkAccumulator | ToonTalkSequencer | ToonTalkTrigger | ToonTalkScheduler, stage: PIXI.Container, renderer: ToonTalkRenderer) {
        this.wasmSprite = wasmSprite;
        this.container = new PIXI.Container();
        this.graphics = new PIXI.Graphics();
        this.renderer = renderer;

        // Position container to match WASM sprite
        this.container.x = wasmSprite.getX();
        this.container.y = wasmSprite.getY();

        // Add graphics to container
        this.container.addChild(this.graphics);

        // Draw the sprite with original ToonTalk textures
        this.drawSprite();

        // Make container interactive
        this.container.eventMode = 'static';
        this.container.cursor = 'pointer';

        // Add interaction handlers
        this.setupInteractions();

        // Add container to stage
        stage.addChild(this.container);

        // Register with game engine systems (if available)
        this.registerWithGameEngine();

        console.log('[WasmSpriteView] Created view for WASM sprite');
    }

    /**
     * Register this sprite with the game engine's collision and entity systems
     */
    private registerWithGameEngine(): void {
        const gameEngine = getGameEngine();
        if (!gameEngine.isInitialized()) return;

        try {
            // Register collision bounds
            const x = this.wasmSprite.getX();
            const y = this.wasmSprite.getY();
            const width = this.wasmSprite.getWidth();
            const height = this.wasmSprite.getHeight();

            this.collisionId = gameEngine.registerCollider(x, y, width, height);

            // Register entity
            const type = this.getObjectType();
            this.entityId = gameEngine.createEntity(type);

            // Store initial data
            if (type === 'number') {
                const num = this.wasmSprite as ToonTalkNumber;
                gameEngine.setEntityComponent(this.entityId, 'value', num.getValue());
            }

            console.log(`[WasmSpriteView] Registered ${type} with collision ID ${this.collisionId}, entity ID ${this.entityId}`);
        } catch (e) {
            // Game engine systems not available (needs WASM rebuild)
        }
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
        if ('getScaleFactor' in this.wasmSprite && 'expand' in this.wasmSprite) return 'expander';
        if ('getCopyCount' in this.wasmSprite && 'makeCopy' in this.wasmSprite) return 'copier';
        if ('getErasedCount' in this.wasmSprite && 'getEraseProgress' in this.wasmSprite) return 'eraser';
        if ('getNumHoles' in this.wasmSprite && 'getFilledCount' in this.wasmSprite && 'setHoleFilled' in this.wasmSprite) return 'cubby';
        if ('getKeyBinding' in this.wasmSprite && 'press' in this.wasmSprite && 'getPressCount' in this.wasmSprite) return 'button';
        if ('push' in this.wasmSprite && 'pop' in this.wasmSprite && 'peek' in this.wasmSprite && 'getCapacity' in this.wasmSprite) return 'stack';
        if ('doFlip' in this.wasmSprite && 'getRotationAngle' in this.wasmSprite && 'nextMode' in this.wasmSprite) return 'flipper';
        if ('getPercentage' in this.wasmSprite && 'isWarning' in this.wasmSprite && 'isCritical' in this.wasmSprite) return 'meter';
        if ('beep' in this.wasmSprite && 'getFrequency' in this.wasmSprite && 'startContinuous' in this.wasmSprite) return 'beeper';
        if ('connect' in this.wasmSprite && 'getSourceId' in this.wasmSprite && 'getTargetId' in this.wasmSprite) return 'connector';
        if ('start' in this.wasmSprite && 'pause' in this.wasmSprite && 'getProgress' in this.wasmSprite && 'getDuration' in this.wasmSprite) return 'timer';
        if ('increment' in this.wasmSprite && 'decrement' in this.wasmSprite && 'getStep' in this.wasmSprite) return 'counter';
        if ('recordSample' in this.wasmSprite && 'getSampleRate' in this.wasmSprite && 'isSampling' in this.wasmSprite) return 'sampler';
        if ('getValueA' in this.wasmSprite && 'getValueB' in this.wasmSprite && 'isEqual' in this.wasmSprite) return 'comparator';
        if ('generate' in this.wasmSprite && 'getSeed' in this.wasmSprite && 'getGenerationCount' in this.wasmSprite) return 'randomizer';
        if ('logEntry' in this.wasmSprite && 'getDebugCount' in this.wasmSprite && 'getWarningCount' in this.wasmSprite) return 'logger';
        if ('processValue' in this.wasmSprite && 'getMinThreshold' in this.wasmSprite && 'getPassedCount' in this.wasmSprite) return 'filter';
        if ('accumulate' in this.wasmSprite && 'getSum' in this.wasmSprite && 'getAverage' in this.wasmSprite) return 'accumulator';
        if ('nextStep' in this.wasmSprite && 'getCurrentStep' in this.wasmSprite && 'getLoopCount' in this.wasmSprite) return 'sequencer';
        if ('checkTrigger' in this.wasmSprite && 'arm' in this.wasmSprite && 'isLatched' in this.wasmSprite) return 'trigger';
        if ('update' in this.wasmSprite && 'getDelay' in this.wasmSprite && 'getElapsedTime' in this.wasmSprite) return 'scheduler';
        return 'sprite';
    }

    /**
     * Draw a visual representation of the sprite
     * Uses Graphics primitives styled like the original ToonTalk
     */
    private drawSprite(): void {
        const type = this.getObjectType();

        this.graphics.clear();

        // Try to load texture from M25 sprites first
        const textureManager = getTextureManager();
        if (textureManager.isLoaded()) {
            const texture = textureManager.getTexture(type);
            if (texture) {
                // Hide graphics when using texture
                this.graphics.visible = false;

                // Create or update sprite with texture
                if (!this.sprite) {
                    this.sprite = new PIXI.Sprite(texture);
                    this.sprite.anchor.set(0.5);
                    this.container.addChild(this.sprite);
                } else {
                    this.sprite.texture = texture;
                }

                // Add text overlay for number/text pads
                this.addTextOverlay(type);
                return;
            }
        }

        // Fallback: Draw using Graphics if no texture available
        // Make sure graphics is visible for fallback rendering
        this.graphics.visible = true;

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
            case 'expander':
                this.drawExpander();
                break;
            case 'copier':
                this.drawCopier();
                break;
            case 'eraser':
                this.drawEraser();
                break;
            case 'cubby':
                this.drawCubby();
                break;
            case 'button':
                this.drawButton();
                break;
            case 'stack':
                this.drawStack();
                break;
            case 'flipper':
                this.drawFlipper();
                break;
            case 'meter':
                this.drawMeter();
                break;
            case 'beeper':
                this.drawBeeper();
                break;
            case 'connector':
                this.drawConnector();
                break;
            case 'timer':
                this.drawTimer();
                break;
            case 'counter':
                this.drawCounter();
                break;
            case 'sampler':
                this.drawSampler();
                break;
            case 'comparator':
                this.drawComparator();
                break;
            case 'randomizer':
                this.drawRandomizer();
                break;
            case 'logger':
                this.drawLogger();
                break;
            case 'filter':
                this.drawFilter();
                break;
            case 'accumulator':
                this.drawAccumulator();
                break;
            case 'sequencer':
                this.drawSequencer();
                break;
            case 'trigger':
                this.drawTrigger();
                break;
            case 'scheduler':
                this.drawScheduler();
                break;
            default:
                this.drawGenericSprite();
                break;
        }
    }

    /**
     * Add text overlay for numbers and text when using textures
     * Matches original ToonTalk rendering with fractions, shrinking decimals, etc.
     */
    private addTextOverlay(type: string): void {
        // Remove old text display if it exists
        if (this.textDisplay) {
            this.textDisplay.destroy();
            this.textDisplay = null;
        }

        if (type === 'number') {
            this.renderNumberText();
        } else if (type === 'text') {
            this.renderTextPad();
        }
    }

    /**
     * Render number text with proper fraction formatting
     * Original ToonTalk format: "1 1/2" displays as:
     *   1  1
     *      -
     *      2
     */
    private renderNumberText(): void {
        const num = this.wasmSprite as ToonTalkNumber;
        const numberStr = num.toString();
        const width = this.sprite?.width || 152;
        const height = this.sprite?.height || 198;

        // Check for fraction format: "integer numerator/denominator" or "numerator/denominator"
        const spaceIndex = numberStr.indexOf(' ');
        const slashIndex = numberStr.indexOf('/');

        if (slashIndex > 0) {
            // Fraction display
            const container = new PIXI.Container();

            if (spaceIndex > 0 && spaceIndex < slashIndex) {
                // Mixed fraction: "1 1/2"
                const integerPart = numberStr.substring(0, spaceIndex);
                const numerator = numberStr.substring(spaceIndex + 1, slashIndex);
                const denominator = numberStr.substring(slashIndex + 1);

                // Integer part (larger, on left)
                const intText = new PIXI.Text(integerPart, {
                    fontSize: Math.min(48, height * 0.35),
                    fill: 0x000000,
                    fontWeight: 'bold',
                    fontFamily: 'Arial'
                });
                intText.x = -width * 0.25;
                intText.y = -intText.height / 2;
                container.addChild(intText);

                // Fraction part (smaller, on right, vertically stacked)
                const fractionSize = Math.min(32, height * 0.25);

                const numText = new PIXI.Text(numerator, {
                    fontSize: fractionSize,
                    fill: 0x000000,
                    fontWeight: 'bold',
                    fontFamily: 'Arial'
                });
                numText.anchor.set(0.5, 1);
                numText.x = width * 0.15;
                numText.y = -5;
                container.addChild(numText);

                // Fraction line
                const line = new PIXI.Graphics();
                line.lineStyle(2, 0x000000);
                line.moveTo(-fractionSize * 0.6, 0);
                line.lineTo(fractionSize * 0.6, 0);
                line.x = width * 0.15;
                line.y = 0;
                container.addChild(line);

                const denomText = new PIXI.Text(denominator, {
                    fontSize: fractionSize,
                    fill: 0x000000,
                    fontWeight: 'bold',
                    fontFamily: 'Arial'
                });
                denomText.anchor.set(0.5, 0);
                denomText.x = width * 0.15;
                denomText.y = 5;
                container.addChild(denomText);

            } else {
                // Simple fraction: "1/2"
                const numerator = numberStr.substring(0, slashIndex);
                const denominator = numberStr.substring(slashIndex + 1);

                const fractionSize = Math.min(36, height * 0.3);

                const numText = new PIXI.Text(numerator, {
                    fontSize: fractionSize,
                    fill: 0x000000,
                    fontWeight: 'bold',
                    fontFamily: 'Arial'
                });
                numText.anchor.set(0.5, 1);
                numText.y = -8;
                container.addChild(numText);

                // Fraction line
                const lineWidth = Math.max(numText.width, fractionSize * 1.2);
                const line = new PIXI.Graphics();
                line.lineStyle(2, 0x000000);
                line.moveTo(-lineWidth / 2, 0);
                line.lineTo(lineWidth / 2, 0);
                container.addChild(line);

                const denomText = new PIXI.Text(denominator, {
                    fontSize: fractionSize,
                    fill: 0x000000,
                    fontWeight: 'bold',
                    fontFamily: 'Arial'
                });
                denomText.anchor.set(0.5, 0);
                denomText.y = 8;
                container.addChild(denomText);
            }

            container.x = width / 2;
            container.y = height / 2;
            this.container.addChild(container);
            this.textDisplay = container;

        } else {
            // Regular number or decimal - use simple centered display
            const fontSize = Math.min(40, height * 0.3);
            const valueText = new PIXI.Text(numberStr, {
                fontSize: fontSize,
                fill: 0x000000,
                fontWeight: 'bold',
                fontFamily: 'Arial'
            });
            valueText.anchor.set(0.5);
            valueText.x = width / 2;
            valueText.y = height / 2;
            this.container.addChild(valueText);
            this.textDisplay = valueText;
        }
    }

    /**
     * Render text pad with proper formatting
     */
    private renderTextPad(): void {
        const txt = this.wasmSprite as ToonTalkText;
        const textContent = txt.getText() || "";
        const width = this.sprite?.width || 152;
        const height = this.sprite?.height || 198;

        if (textContent) {
            const fontSize = Math.min(24, height * 0.15);
            const valueText = new PIXI.Text(textContent, {
                fontSize: fontSize,
                fill: 0x000000,
                fontWeight: 'normal',
                fontFamily: 'Arial',
                wordWrap: true,
                wordWrapWidth: width - 30,
                align: 'left',
                breakWords: true
            });
            valueText.anchor.set(0.5);
            valueText.x = width / 2;
            valueText.y = height / 2;
            this.container.addChild(valueText);
            this.textDisplay = valueText;
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

    private drawCubby(): void {
        const cubby = this.wasmSprite as ToonTalkCubby;
        const numHoles = cubby.getNumHoles();
        const filledCount = cubby.getFilledCount();

        // Cubby body (multi-compartment container - purple/violet)
        const width = 80 + (numHoles * 15);
        this.graphics.beginFill(0x9370DB);
        this.graphics.drawRoundedRect(-width / 2, -40, width, 80, 8);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x663399);
        this.graphics.drawRoundedRect(-width / 2, -40, width, 80, 8);

        // Label at top
        const label = new PIXI.Text('Cubby', {
            fontSize: 12,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = -30;
        this.graphics.addChild(label);

        // Draw compartments (holes) with labels
        const holeSpacing = (width - 20) / (numHoles + 1);
        for (let i = 0; i < numHoles; i++) {
            const holeX = -width / 2 + 10 + (i + 1) * holeSpacing;
            const holeY = 5;
            const isFilled = cubby.isHoleFilled(i);

            // Compartment background
            this.graphics.beginFill(isFilled ? 0xFFD700 : 0x4B0082, isFilled ? 1 : 0.6);
            this.graphics.drawRoundedRect(holeX - 12, holeY - 15, 24, 30, 4);
            this.graphics.endFill();

            // Compartment border
            this.graphics.lineStyle(2, isFilled ? 0xFFD700 : 0x663399);
            this.graphics.drawRoundedRect(holeX - 12, holeY - 15, 24, 30, 4);

            // Label (if exists)
            const labelChar = cubby.getHoleLabel(i);
            if (labelChar && labelChar !== '\0') {
                const labelText = new PIXI.Text(labelChar, {
                    fontSize: 10,
                    fill: isFilled ? 0x000000 : 0xFFFFFF,
                    fontWeight: 'bold'
                });
                labelText.anchor.set(0.5);
                labelText.x = holeX;
                labelText.y = holeY;
                this.graphics.addChild(labelText);
            }
        }

        // Status text
        const statusText = new PIXI.Text(`${filledCount} / ${numHoles} filled`, {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        statusText.anchor.set(0.5);
        statusText.y = 30;
        this.graphics.addChild(statusText);
    }

    private drawButton(): void {
        const button = this.wasmSprite as ToonTalkButton;
        const state = button.getStateInt();
        const keyBinding = button.getKeyBinding();
        const pressCount = button.getPressCount();
        const isEnabled = button.isEnabled();

        // Button states: NORMAL=0, HOVERED=1, PRESSED=2, DISABLED=3
        const stateColors = [
            0x4169E1, // NORMAL - royal blue
            0x1E90FF, // HOVERED - dodger blue (brighter)
            0x0000CD, // PRESSED - medium blue (darker)
            0x696969  // DISABLED - dim gray
        ];

        const buttonColor = stateColors[state] || 0x4169E1;

        // Button shape (rounded rectangle with depth effect)
        const yOffset = state === 2 ? 3 : 0; // Pressed = pushed down

        // Shadow (if not pressed)
        if (state !== 2) {
            this.graphics.beginFill(0x000000, 0.3);
            this.graphics.drawRoundedRect(-35, -17 + 3, 70, 34, 8);
            this.graphics.endFill();
        }

        // Button body
        this.graphics.beginFill(buttonColor);
        this.graphics.drawRoundedRect(-35, -17 + yOffset, 70, 34, 8);
        this.graphics.endFill();

        // Border (glow if hovered)
        this.graphics.lineStyle(state === 1 ? 4 : 2, state === 1 ? 0xFFFFFF : 0x000000, state === 1 ? 0.8 : 1);
        this.graphics.drawRoundedRect(-35, -17 + yOffset, 70, 34, 8);

        // Highlight (top of button for 3D effect)
        if (state !== 2) {
            this.graphics.lineStyle(2, 0xFFFFFF, 0.5);
            this.graphics.moveTo(-30, -12 + yOffset);
            this.graphics.lineTo(30, -12 + yOffset);
        }

        // Key binding display
        if (keyBinding && keyBinding !== '\0') {
            const keyText = new PIXI.Text(keyBinding, {
                fontSize: 18,
                fill: isEnabled ? 0xFFFFFF : 0x888888,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 3
            });
            keyText.anchor.set(0.5);
            keyText.y = yOffset;
            this.graphics.addChild(keyText);
        } else {
            const icon = new PIXI.Text('●', {
                fontSize: 20,
                fill: isEnabled ? 0xFFFFFF : 0x888888
            });
            icon.anchor.set(0.5);
            icon.y = yOffset;
            this.graphics.addChild(icon);
        }

        // Label
        const label = new PIXI.Text('Button', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 30;
        this.graphics.addChild(label);

        // Press count (if pressed)
        if (pressCount > 0) {
            const countText = new PIXI.Text(`×${pressCount}`, {
                fontSize: 8,
                fill: 0x00FF00,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2
            });
            countText.anchor.set(0.5);
            countText.y = -28;
            this.graphics.addChild(countText);
        }

        // State indicator
        const stateNames = ['NORMAL', 'HOVER', 'PRESS', 'DISABLED'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 7,
            fill: state === 2 ? 0xFFFF00 : (state === 3 ? 0xFF0000 : 0xCCCCCC),
            stroke: 0x000000,
            strokeThickness: 1
        });
        stateText.anchor.set(0.5);
        stateText.y = 40;
        this.graphics.addChild(stateText);
    }

    private drawStack(): void {
        const stack = this.wasmSprite as ToonTalkStack;
        const capacity = stack.getCapacity();
        const count = stack.getCount();
        const fullness = stack.getFullness();

        // Stack container (vertical structure - orange/brown)
        this.graphics.beginFill(0xD2691E);
        this.graphics.drawRoundedRect(-40, -60, 80, 120, 5);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x8B4513);
        this.graphics.drawRoundedRect(-40, -60, 80, 120, 5);

        // Label at top
        const label = new PIXI.Text('Stack', {
            fontSize: 12,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = -48;
        this.graphics.addChild(label);

        // Draw stack items (from bottom to top)
        const itemHeight = 8;
        const maxVisibleItems = 10;
        const visibleItems = Math.min(count, maxVisibleItems);

        for (let i = 0; i < visibleItems; i++) {
            const itemY = 50 - (i * itemHeight);
            const itemColor = i === visibleItems - 1 ? 0xFFD700 : 0xFF8C00; // Top item is gold

            this.graphics.beginFill(itemColor);
            this.graphics.drawRect(-35, itemY - itemHeight, 70, itemHeight - 1);
            this.graphics.endFill();

            // Item border
            this.graphics.lineStyle(1, 0x654321);
            this.graphics.drawRect(-35, itemY - itemHeight, 70, itemHeight - 1);
        }

        // If more items than visible, show indicator
        if (count > maxVisibleItems) {
            const moreText = new PIXI.Text(`+${count - maxVisibleItems}`, {
                fontSize: 8,
                fill: 0xFFFF00,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2
            });
            moreText.anchor.set(0.5);
            moreText.y = 50 - (maxVisibleItems * itemHeight) - 8;
            this.graphics.addChild(moreText);
        }

        // Fullness bar
        const barHeight = 100 * fullness;
        this.graphics.beginFill(0x90EE90, 0.4);
        this.graphics.drawRect(-38, 58 - barHeight, 6, barHeight);
        this.graphics.endFill();

        // Count display
        const countText = new PIXI.Text(`${count}`, {
            fontSize: 18,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 3
        });
        countText.anchor.set(0.5);
        countText.y = -15;
        this.graphics.addChild(countText);

        // Capacity text
        const capacityText = new PIXI.Text(`/ ${capacity}`, {
            fontSize: 10,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 2
        });
        capacityText.anchor.set(0.5);
        capacityText.y = 5;
        this.graphics.addChild(capacityText);

        // LIFO indicator
        const lifoText = new PIXI.Text('LIFO', {
            fontSize: 8,
            fill: 0xFFFF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1
        });
        lifoText.anchor.set(0.5);
        lifoText.y = 45;
        this.graphics.addChild(lifoText);

        // Arrow pointing up (showing push direction)
        if (count < capacity) {
            this.graphics.lineStyle(2, 0x00FF00);
            const arrowY = 50 - (visibleItems * itemHeight) - 15;
            this.graphics.moveTo(-8, arrowY + 5);
            this.graphics.lineTo(0, arrowY - 5);
            this.graphics.lineTo(8, arrowY + 5);
        }
    }

    private drawFlipper(): void {
        const flipper = this.wasmSprite as ToonTalkFlipper;
        const mode = flipper.getModeInt();
        const angle = flipper.getRotationAngle();
        const flipCount = flipper.getFlipCount();
        const hasAttached = flipper.hasAttached();

        // Flipper body (paddle/spatula shape - cyan/turquoise)
        this.graphics.beginFill(0x40E0D0);
        this.graphics.drawRoundedRect(-35, -20, 70, 40, 8);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(2, 0x20B2AA);
        this.graphics.drawRoundedRect(-35, -20, 70, 40, 8);

        // Mode indicator icons
        const modeIcons = ['↔️', '↕️', '↻', '↺']; // HORIZONTAL, VERTICAL, ROTATE_CW, ROTATE_CCW
        const modeNames = ['H-FLIP', 'V-FLIP', 'CW', 'CCW'];

        // Central icon
        const icon = new PIXI.Text(modeIcons[mode] || '?', {
            fontSize: 24
        });
        icon.anchor.set(0.5);
        icon.y = -2;
        this.graphics.addChild(icon);

        // Rotation angle display (if in rotate mode)
        if (mode === 2 || mode === 3) {
            const angleText = new PIXI.Text(`${angle.toFixed(0)}°`, {
                fontSize: 12,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2
            });
            angleText.anchor.set(0.5);
            angleText.y = 12;
            this.graphics.addChild(angleText);
        }

        // Flip count badge
        if (flipCount > 0) {
            const countText = new PIXI.Text(`×${flipCount}`, {
                fontSize: 10,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2,
                backgroundColor: 0xFF6347,
                padding: 2
            });
            countText.anchor.set(0.5);
            countText.x = 25;
            countText.y = -15;
            this.graphics.addChild(countText);
        }

        // Attached indicator
        if (hasAttached) {
            const attachedIcon = new PIXI.Text('📎', {
                fontSize: 16
            });
            attachedIcon.anchor.set(0.5);
            attachedIcon.x = -25;
            attachedIcon.y = -15;
            this.graphics.addChild(attachedIcon);
        }

        // Label
        const label = new PIXI.Text('Flipper', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 35;
        this.graphics.addChild(label);

        // Mode text
        const modeText = new PIXI.Text(modeNames[mode] || '?', {
            fontSize: 8,
            fill: 0xFFFF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1
        });
        modeText.anchor.set(0.5);
        modeText.y = -30;
        this.graphics.addChild(modeText);
    }

    private drawMeter(): void {
        const meter = this.wasmSprite as ToonTalkMeter;
        const state = meter.getStateInt();
        const value = meter.getValue();
        const percentage = meter.getPercentage();

        // Meter body (gauge container - dark gray)
        this.graphics.beginFill(0x2F4F4F);
        this.graphics.drawRoundedRect(-30, -60, 60, 120, 5);
        this.graphics.endFill();

        // Border (changes color based on state)
        const stateColors = [
            0x00FF00, // NORMAL - green
            0xFFFF00, // WARNING - yellow
            0xFF8C00, // CRITICAL - orange
            0xFF0000  // MAXED - red
        ];
        this.graphics.lineStyle(3, stateColors[state] || 0x00FF00);
        this.graphics.drawRoundedRect(-30, -60, 60, 120, 5);

        // Background gauge bar
        this.graphics.beginFill(0x1C1C1C);
        this.graphics.drawRect(-25, -50, 50, 90);
        this.graphics.endFill();

        // Fill bar (color changes with state)
        const fillHeight = 90 * (percentage / 100);
        this.graphics.beginFill(stateColors[state] || 0x00FF00);
        this.graphics.drawRect(-25, 40 - fillHeight, 50, fillHeight);
        this.graphics.endFill();

        // Tick marks
        for (let i = 0; i <= 10; i++) {
            const tickY = 40 - (i * 9);
            const tickWidth = i % 5 === 0 ? 8 : 4;
            this.graphics.lineStyle(1, 0x808080);
            this.graphics.moveTo(-25 - tickWidth, tickY);
            this.graphics.lineTo(-25, tickY);
            this.graphics.moveTo(25, tickY);
            this.graphics.lineTo(25 + tickWidth, tickY);
        }

        // Percentage display
        const percentText = new PIXI.Text(`${percentage.toFixed(0)}%`, {
            fontSize: 16,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 3
        });
        percentText.anchor.set(0.5);
        percentText.y = -10;
        this.graphics.addChild(percentText);

        // Value display
        const valueText = new PIXI.Text(`${value.toFixed(1)}`, {
            fontSize: 10,
            fill: 0xCCCCCC,
            stroke: 0x000000,
            strokeThickness: 1
        });
        valueText.anchor.set(0.5);
        valueText.y = 5;
        this.graphics.addChild(valueText);

        // Label
        const label = new PIXI.Text('Meter', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 50;
        this.graphics.addChild(label);

        // State indicator
        const stateNames = ['NORMAL', 'WARN', 'CRIT', 'MAX'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: stateColors[state] || 0xCCCCCC,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -55;
        this.graphics.addChild(stateText);

        // Warning/Critical/Maxed icons
        if (state === 1) { // WARNING
            const warnIcon = new PIXI.Text('⚠️', {
                fontSize: 12
            });
            warnIcon.anchor.set(0.5);
            warnIcon.x = 20;
            warnIcon.y = -30;
            this.graphics.addChild(warnIcon);
        } else if (state === 2) { // CRITICAL
            const critIcon = new PIXI.Text('❗', {
                fontSize: 14
            });
            critIcon.anchor.set(0.5);
            critIcon.x = 20;
            critIcon.y = -30;
            this.graphics.addChild(critIcon);
        } else if (state === 3) { // MAXED
            const maxIcon = new PIXI.Text('🔴', {
                fontSize: 12
            });
            maxIcon.anchor.set(0.5);
            maxIcon.x = 20;
            maxIcon.y = -30;
            this.graphics.addChild(maxIcon);
        }
    }

    private drawBeeper(): void {
        const beeper = this.wasmSprite as ToonTalkBeeper;
        const state = beeper.getStateInt();
        const beepCount = beeper.getBeepCount();
        const frequency = beeper.getFrequency();
        const isBeeping = beeper.isBeeping();

        // Beeper body (speaker/bell shape - orange)
        const bodyColor = state === 0 ? 0xFFA500 : (state === 1 ? 0xFF8C00 : 0xFF6347);
        this.graphics.beginFill(bodyColor);
        this.graphics.drawCircle(0, 0, 25);
        this.graphics.endFill();

        // Border (glows when beeping)
        this.graphics.lineStyle(state !== 0 ? 4 : 2, state !== 0 ? 0xFFFF00 : 0xCC8400, state !== 0 ? 0.8 : 1);
        this.graphics.drawCircle(0, 0, 25);

        // Speaker cone (concentric circles)
        for (let i = 1; i <= 3; i++) {
            const radius = i * 6;
            this.graphics.beginFill(0xFFD700, 0.3);
            this.graphics.drawCircle(0, 0, radius);
            this.graphics.endFill();
        }

        // Sound waves (if beeping or continuous)
        if (isBeeping) {
            for (let i = 0; i < 3; i++) {
                const distance = 30 + i * 10;
                const alpha = 0.6 - i * 0.2;
                this.graphics.lineStyle(2, 0xFFFF00, alpha);
                // Draw arc (right side)
                this.graphics.arc(0, 0, distance, -Math.PI / 4, Math.PI / 4);
                // Draw arc (left side)
                this.graphics.arc(0, 0, distance, (3 * Math.PI) / 4, (5 * Math.PI) / 4);
            }
        }

        // Bell icon in center
        const icon = new PIXI.Text(state === 2 ? '🔊' : '🔔', {
            fontSize: 20
        });
        icon.anchor.set(0.5);
        icon.y = -2;
        this.graphics.addChild(icon);

        // Beep count
        if (beepCount > 0) {
            const countText = new PIXI.Text(`${beepCount}`, {
                fontSize: 12,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2,
                backgroundColor: 0xFF0000,
                padding: 2
            });
            countText.anchor.set(0.5);
            countText.x = 18;
            countText.y = -18;
            this.graphics.addChild(countText);
        }

        // Label
        const label = new PIXI.Text('Beeper', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 40;
        this.graphics.addChild(label);

        // Frequency display
        const freqText = new PIXI.Text(`${frequency.toFixed(0)}Hz`, {
            fontSize: 8,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 1
        });
        freqText.anchor.set(0.5);
        freqText.y = -32;
        this.graphics.addChild(freqText);

        // State indicator
        const stateNames = ['SILENT', 'BEEP', 'CONT'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 7,
            fill: state === 0 ? 0xCCCCCC : 0xFFFF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1
        });
        stateText.anchor.set(0.5);
        stateText.y = 50;
        this.graphics.addChild(stateText);
    }

    private drawConnector(): void {
        const connector = this.wasmSprite as ToonTalkConnector;
        const state = connector.getStateInt();
        const connectionCount = connector.getConnectionCount();
        const isConnected = connector.isConnected();
        const hasError = connector.hasError();

        // State colors
        const stateColors = [
            0x808080, // DISCONNECTED - gray
            0xFFFF00, // CONNECTING - yellow
            0x00FF00, // CONNECTED - green
            0xFF0000  // ERROR - red
        ];

        // Connector body (plug/socket shape - yellow/gold)
        this.graphics.beginFill(0xFFD700);
        this.graphics.drawRoundedRect(-30, -20, 60, 40, 5);
        this.graphics.endFill();

        // Border (color based on state)
        this.graphics.lineStyle(3, stateColors[state] || 0x808080);
        this.graphics.drawRoundedRect(-30, -20, 60, 40, 5);

        // Connection ports (two circles)
        const portY = 0;
        this.graphics.beginFill(isConnected ? 0x00FF00 : 0x404040);
        this.graphics.drawCircle(-15, portY, 8);
        this.graphics.endFill();

        this.graphics.beginFill(isConnected ? 0x00FF00 : 0x404040);
        this.graphics.drawCircle(15, portY, 8);
        this.graphics.endFill();

        // Connection line (if connected)
        if (isConnected) {
            this.graphics.lineStyle(3, 0x00FF00);
            this.graphics.moveTo(-15, portY);
            this.graphics.lineTo(15, portY);
        } else if (state === 1) { // CONNECTING - animated dashed line
            this.graphics.lineStyle(2, 0xFFFF00, 0.7);
            for (let i = -15; i < 15; i += 8) {
                this.graphics.moveTo(i, portY);
                this.graphics.lineTo(i + 4, portY);
            }
        }

        // Connection icon
        const icon = new PIXI.Text(state === 2 ? '🔗' : (state === 3 ? '⚠️' : '○○'), {
            fontSize: 16
        });
        icon.anchor.set(0.5);
        icon.y = 0;
        this.graphics.addChild(icon);

        // Label
        const label = new PIXI.Text('Connector', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 35;
        this.graphics.addChild(label);

        // Connection count
        if (connectionCount > 0) {
            const countText = new PIXI.Text(`×${connectionCount}`, {
                fontSize: 10,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 2,
                backgroundColor: 0x00FF00,
                padding: 2
            });
            countText.anchor.set(0.5);
            countText.x = 22;
            countText.y = -18;
            this.graphics.addChild(countText);
        }

        // State indicator
        const stateNames = ['DISC', 'CONN-ING', 'CONN', 'ERROR'];
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

    private drawTimer(): void {
        const timer = this.wasmSprite as ToonTalkTimer;
        const state = timer.getStateInt();
        const duration = timer.getDuration();
        const progress = timer.getProgress();
        const remaining = timer.getRemaining();
        const isLoop = timer.isLoop();

        // Timer body (stopwatch shape - silver/blue)
        const bodyColor = state === 1 ? 0x4169E1 : 0xC0C0C0;
        this.graphics.beginFill(bodyColor);
        this.graphics.drawCircle(0, 0, 35);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x696969);
        this.graphics.drawCircle(0, 0, 35);

        // Progress arc (if running or paused)
        if (progress > 0 && progress <= 1) {
            const startAngle = -Math.PI / 2;
            const endAngle = startAngle + (progress * 2 * Math.PI);
            this.graphics.lineStyle(5, state === 1 ? 0x00FF00 : 0xFFFF00);
            this.graphics.arc(0, 0, 30, startAngle, endAngle);
        }

        // Clock ticks
        for (let i = 0; i < 12; i++) {
            const angle = (i / 12) * Math.PI * 2 - Math.PI / 2;
            const innerRadius = 25;
            const outerRadius = i % 3 === 0 ? 30 : 28;
            const x1 = Math.cos(angle) * innerRadius;
            const y1 = Math.sin(angle) * innerRadius;
            const x2 = Math.cos(angle) * outerRadius;
            const y2 = Math.sin(angle) * outerRadius;
            this.graphics.lineStyle(1, 0x000000);
            this.graphics.moveTo(x1, y1);
            this.graphics.lineTo(x2, y2);
        }

        // Timer hand (shows remaining time)
        const handAngle = (1 - progress) * 2 * Math.PI - Math.PI / 2;
        this.graphics.lineStyle(3, 0xFF0000);
        this.graphics.moveTo(0, 0);
        this.graphics.lineTo(Math.cos(handAngle) * 20, Math.sin(handAngle) * 20);

        // Center dot
        this.graphics.beginFill(0xFF0000);
        this.graphics.drawCircle(0, 0, 3);
        this.graphics.endFill();

        // Remaining time display
        const remainingText = new PIXI.Text(`${remaining.toFixed(1)}s`, {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        remainingText.anchor.set(0.5);
        remainingText.y = 10;
        this.graphics.addChild(remainingText);

        // Label
        const label = new PIXI.Text('Timer', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 50;
        this.graphics.addChild(label);

        // Loop indicator
        if (isLoop) {
            const loopIcon = new PIXI.Text('🔄', {
                fontSize: 12
            });
            loopIcon.anchor.set(0.5);
            loopIcon.x = 28;
            loopIcon.y = -28;
            this.graphics.addChild(loopIcon);
        }

        // State indicator
        const stateNames = ['STOP', 'RUN', 'PAUSE', 'DONE'];
        const stateColors = [0x808080, 0x00FF00, 0xFFFF00, 0xFF6347];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: stateColors[state] || 0xCCCCCC,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -42;
        this.graphics.addChild(stateText);

        // Play/pause/stop icon
        let icon = '⏸️';
        if (state === 0) icon = '⏹️';
        else if (state === 1) icon = '▶️';
        else if (state === 3) icon = '✅';

        const iconText = new PIXI.Text(icon, {
            fontSize: 12
        });
        iconText.anchor.set(0.5);
        iconText.y = -15;
        this.graphics.addChild(iconText);
    }

    private drawCounter(): void {
        const counter = this.wasmSprite as ToonTalkCounter;
        const state = counter.getStateInt();
        const value = counter.getValue();
        const step = counter.getStep();
        const isAtMin = counter.isAtMin();
        const isAtMax = counter.isAtMax();

        // Counter body (digital display shape - dark blue)
        this.graphics.beginFill(0x1C1C3C);
        this.graphics.drawRoundedRect(-40, -40, 80, 80, 8);
        this.graphics.endFill();

        // Border (color based on state)
        const stateColors = [
            0x4169E1, // NORMAL - blue
            0xFF6347, // AT_MIN - red
            0xFF6347, // AT_MAX - red
            0xFF0000  // OVERFLOW - bright red
        ];
        this.graphics.lineStyle(3, stateColors[state] || 0x4169E1);
        this.graphics.drawRoundedRect(-40, -40, 80, 80, 8);

        // Value display (large digital style)
        const valueText = new PIXI.Text(`${value}`, {
            fontSize: 24,
            fill: state === 0 ? 0x00FF00 : 0xFF6347,
            fontWeight: 'bold',
            fontFamily: 'monospace',
            stroke: 0x000000,
            strokeThickness: 3
        });
        valueText.anchor.set(0.5);
        valueText.y = -5;
        this.graphics.addChild(valueText);

        // Increment button (up arrow)
        const upColor = isAtMax ? 0x404040 : 0x00FF00;
        this.graphics.beginFill(upColor, 0.3);
        this.graphics.drawCircle(0, -25, 8);
        this.graphics.endFill();
        this.graphics.lineStyle(2, upColor);
        this.graphics.moveTo(-5, -22);
        this.graphics.lineTo(0, -27);
        this.graphics.lineTo(5, -22);

        // Decrement button (down arrow)
        const downColor = isAtMin ? 0x404040 : 0xFF6347;
        this.graphics.beginFill(downColor, 0.3);
        this.graphics.drawCircle(0, 25, 8);
        this.graphics.endFill();
        this.graphics.lineStyle(2, downColor);
        this.graphics.moveTo(-5, 22);
        this.graphics.lineTo(0, 27);
        this.graphics.lineTo(5, 22);

        // Step indicator
        if (step > 1) {
            const stepText = new PIXI.Text(`±${step}`, {
                fontSize: 8,
                fill: 0xFFFF00,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 1
            });
            stepText.anchor.set(0.5);
            stepText.x = 28;
            stepText.y = 0;
            this.graphics.addChild(stepText);
        }

        // Label
        const label = new PIXI.Text('Counter', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 50;
        this.graphics.addChild(label);

        // State indicator
        const stateNames = ['NORMAL', 'MIN', 'MAX', 'OVER'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: stateColors[state] || 0xCCCCCC,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -35;
        this.graphics.addChild(stateText);

        // Warning icons for limit states
        if (isAtMin) {
            const minIcon = new PIXI.Text('⬇️', {
                fontSize: 12
            });
            minIcon.anchor.set(0.5);
            minIcon.x = -28;
            minIcon.y = 25;
            this.graphics.addChild(minIcon);
        }
        if (isAtMax) {
            const maxIcon = new PIXI.Text('⬆️', {
                fontSize: 12
            });
            maxIcon.anchor.set(0.5);
            maxIcon.x = 28;
            maxIcon.y = -25;
            this.graphics.addChild(maxIcon);
        }
    }

    private drawSampler(): void {
        const sampler = this.wasmSprite as ToonTalkSampler;
        const state = sampler.getStateInt();
        const sampleCount = sampler.getSampleCount();
        const fullness = sampler.getFullness();
        const isSampling = sampler.isSampling();

        // Sampler body (oscilloscope/recorder shape - teal/aqua)
        const bodyColor = state === 1 ? 0x00CED1 : 0x5F9EA0;
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-35, -35, 70, 70, 5);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x2F4F4F);
        this.graphics.drawRoundedRect(-35, -35, 70, 70, 5);

        // Waveform display (simulated)
        if (sampleCount > 0) {
            this.graphics.lineStyle(2, 0x00FF00);
            const waveWidth = 60;
            const waveHeight = 20;
            const samples = Math.min(sampleCount, 20);
            for (let i = 0; i < samples; i++) {
                const x = -30 + (i / samples) * waveWidth;
                const y = Math.sin((i / samples) * Math.PI * 4) * waveHeight;
                if (i === 0) {
                    this.graphics.moveTo(x, y);
                } else {
                    this.graphics.lineTo(x, y);
                }
            }
        }

        // Sampling indicator (pulsing dot when sampling)
        if (isSampling) {
            this.graphics.beginFill(0xFF0000);
            this.graphics.drawCircle(25, -25, 4);
            this.graphics.endFill();
        }

        // Progress bar (fullness)
        this.graphics.beginFill(0x404040, 0.5);
        this.graphics.drawRect(-30, 20, 60, 8);
        this.graphics.endFill();
        this.graphics.beginFill(0x00FF00);
        this.graphics.drawRect(-30, 20, 60 * fullness, 8);
        this.graphics.endFill();

        // Sample count
        const countText = new PIXI.Text(`${sampleCount}`, {
            fontSize: 14,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        countText.anchor.set(0.5);
        countText.y = -10;
        this.graphics.addChild(countText);

        // Label
        const label = new PIXI.Text('Sampler', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 45;
        this.graphics.addChild(label);

        // State indicator
        const stateNames = ['IDLE', 'SAMP', 'PAUSE', 'FULL'];
        const stateColors = [0x808080, 0x00FF00, 0xFFFF00, 0xFF0000];
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

    private drawComparator(): void {
        const comparator = this.wasmSprite as ToonTalkComparator;
        const result = comparator.getResultInt();
        const valueA = comparator.getValueA();
        const valueB = comparator.getValueB();
        const isEqual = comparator.isEqual();

        // Comparator body (balance/scales shape - purple)
        this.graphics.beginFill(0x9370DB);
        this.graphics.drawRoundedRect(-40, -30, 80, 60, 5);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(3, 0x663399);
        this.graphics.drawRoundedRect(-40, -30, 80, 60, 5);

        // Value A (left side)
        const valueAText = new PIXI.Text(`${valueA.toFixed(1)}`, {
            fontSize: 12,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        valueAText.anchor.set(0.5);
        valueAText.x = -20;
        valueAText.y = -10;
        this.graphics.addChild(valueAText);

        // Value B (right side)
        const valueBText = new PIXI.Text(`${valueB.toFixed(1)}`, {
            fontSize: 12,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        valueBText.anchor.set(0.5);
        valueBText.x = 20;
        valueBText.y = -10;
        this.graphics.addChild(valueBText);

        // Comparison symbol in center
        const symbols = ['=', '<', '>', '≠'];
        const symbolColors = [0x00FF00, 0xFF6347, 0xFF6347, 0xFFFF00];
        const symbolText = new PIXI.Text(symbols[result] || '?', {
            fontSize: 20,
            fill: symbolColors[result] || 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 3
        });
        symbolText.anchor.set(0.5);
        symbolText.y = 10;
        this.graphics.addChild(symbolText);

        // Scale visualization (tilt based on comparison)
        const tiltAngle = result === 1 ? -0.2 : (result === 2 ? 0.2 : 0);
        this.graphics.lineStyle(3, 0xFFD700);
        this.graphics.moveTo(-25, -20);
        this.graphics.lineTo(25, -20 + tiltAngle * 50);

        // Label
        const label = new PIXI.Text('Comparator', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 40;
        this.graphics.addChild(label);

        // Result indicator
        const resultNames = ['EQUAL', '<', '>', 'NOT='];
        const resultText = new PIXI.Text(resultNames[result] || '?', {
            fontSize: 8,
            fill: symbolColors[result] || 0xCCCCCC,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        resultText.anchor.set(0.5);
        resultText.y = -25;
        this.graphics.addChild(resultText);
    }

    private drawRandomizer(): void {
        const randomizer = this.wasmSprite as ToonTalkRandomizer;
        const mode = randomizer.getModeInt();
        const currentValue = randomizer.getCurrentValue();
        const generationCount = randomizer.getGenerationCount();

        // Randomizer body (dice/random shape - orange/gold)
        const bodyColor = generationCount > 0 ? 0xFFA500 : 0xFFD700;
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-30, -30, 60, 60, 8);
        this.graphics.endFill();

        // Border (sparkly effect)
        this.graphics.lineStyle(3, 0xFF8C00);
        this.graphics.drawRoundedRect(-30, -30, 60, 60, 8);

        // Mode-based icon
        const modeIcons = ['🎲', '🔢', '🔘', '⚅'];
        const icon = new PIXI.Text(modeIcons[mode] || '?', {
            fontSize: 24
        });
        icon.anchor.set(0.5);
        icon.y = -8;
        this.graphics.addChild(icon);

        // Current value display
        let displayValue = '';
        switch (mode) {
            case 0: // UNIFORM
                displayValue = currentValue.toFixed(2);
                break;
            case 1: // INTEGER
            case 3: // DICE
                displayValue = Math.floor(currentValue).toString();
                break;
            case 2: // BOOLEAN
                displayValue = currentValue === 0 ? 'F' : 'T';
                break;
        }

        const valueText = new PIXI.Text(displayValue, {
            fontSize: 14,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2,
            fontFamily: 'monospace'
        });
        valueText.anchor.set(0.5);
        valueText.y = 12;
        this.graphics.addChild(valueText);

        // Sparkles (if generating)
        if (generationCount > 0) {
            for (let i = 0; i < 4; i++) {
                const angle = (i / 4) * Math.PI * 2 + (Date.now() / 500);
                const distance = 25;
                const x = Math.cos(angle) * distance;
                const y = Math.sin(angle) * distance;
                this.graphics.beginFill(0xFFFFFF, 0.7);
                this.graphics.drawCircle(x, y, 2);
                this.graphics.endFill();
            }
        }

        // Label
        const label = new PIXI.Text('Random', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 40;
        this.graphics.addChild(label);

        // Mode indicator
        const modeNames = ['UNIFORM', 'INT', 'BOOL', 'DICE'];
        const modeText = new PIXI.Text(modeNames[mode] || '?', {
            fontSize: 8,
            fill: 0xFFFF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        modeText.anchor.set(0.5);
        modeText.y = -25;
        this.graphics.addChild(modeText);

        // Generation count
        if (generationCount > 0) {
            const countText = new PIXI.Text(`×${generationCount}`, {
                fontSize: 8,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 1,
                backgroundColor: 0xFF6347,
                padding: 2
            });
            countText.anchor.set(0.5);
            countText.x = 22;
            countText.y = -22;
            this.graphics.addChild(countText);
        }
    }

    private drawLogger(): void {
        const logger = this.wasmSprite as ToonTalkLogger;
        const state = logger.getStateInt();
        const entryCount = logger.getEntryCount();
        const maxEntries = logger.getMaxEntries();
        const fullness = logger.getFullness();
        const debugCount = logger.getDebugCount();
        const infoCount = logger.getInfoCount();
        const warningCount = logger.getWarningCount();
        const errorCount = logger.getErrorCount();
        const isLogging = logger.isLogging();

        // Logger body (console/terminal shape - dark gray/black)
        const bodyColor = state === 1 ? 0x2F4F4F : 0x1C1C1C; // Darker when logging
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-40, -35, 80, 70, 5);
        this.graphics.endFill();

        // Border with logging indicator
        const borderColor = state === 1 ? 0x00FF00 : (state === 2 ? 0xFFFF00 : 0x808080);
        this.graphics.lineStyle(3, borderColor);
        this.graphics.drawRoundedRect(-40, -35, 80, 70, 5);

        // Screen/display area
        this.graphics.beginFill(0x000000, 0.8);
        this.graphics.drawRoundedRect(-35, -30, 70, 45, 3);
        this.graphics.endFill();

        // Log entries visualization (color-coded bars)
        if (entryCount > 0) {
            const barHeight = 2;
            const maxBars = 10;
            const visibleCount = Math.min(entryCount, maxBars);

            // Draw recent log entries as colored lines
            for (let i = 0; i < visibleCount; i++) {
                const y = -25 + (i * 4);
                // Color based on most recent log levels (simplified visualization)
                let color = 0x00FF00; // Default green for DEBUG/INFO
                if (errorCount > 0 && i < Math.floor(visibleCount * errorCount / entryCount)) {
                    color = 0xFF0000; // Red for ERROR
                } else if (warningCount > 0 && i < Math.floor(visibleCount * (warningCount + errorCount) / entryCount)) {
                    color = 0xFFFF00; // Yellow for WARNING
                }

                this.graphics.lineStyle(barHeight, color, 0.7);
                this.graphics.moveTo(-32, y);
                this.graphics.lineTo(32, y);
            }
        }

        // Entry count display
        const countText = new PIXI.Text(`${entryCount}/${maxEntries}`, {
            fontSize: 10,
            fill: 0x00FF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2,
            fontFamily: 'monospace'
        });
        countText.anchor.set(0.5);
        countText.y = 5;
        this.graphics.addChild(countText);

        // Log level counts (compact display)
        const statsText = new PIXI.Text(`E:${errorCount} W:${warningCount}`, {
            fontSize: 8,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 1,
            fontFamily: 'monospace'
        });
        statsText.anchor.set(0.5);
        statsText.y = 18;
        this.graphics.addChild(statsText);

        // Recording indicator (pulsing dot when logging)
        if (isLogging) {
            const pulse = Math.sin(Date.now() / 200) * 0.5 + 0.5;
            this.graphics.beginFill(0xFF0000, 0.5 + pulse * 0.5);
            this.graphics.drawCircle(32, -27, 4);
            this.graphics.endFill();
        }

        // Fullness progress bar
        this.graphics.beginFill(0x404040, 0.5);
        this.graphics.drawRect(-35, 25, 70, 6);
        this.graphics.endFill();
        if (fullness > 0) {
            const fillColor = fullness >= 1.0 ? 0xFF0000 : (fullness >= 0.75 ? 0xFFFF00 : 0x00FF00);
            this.graphics.beginFill(fillColor);
            this.graphics.drawRect(-35, 25, 70 * fullness, 6);
            this.graphics.endFill();
        }

        // Label
        const label = new PIXI.Text('Logger', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 42;
        this.graphics.addChild(label);
    }

    private drawFilter(): void {
        const filter = this.wasmSprite as ToonTalkFilter;
        const mode = filter.getModeInt();
        const state = filter.getStateInt();
        const passedCount = filter.getPassedCount();
        const blockedCount = filter.getBlockedCount();
        const inputValue = filter.getInputValue();
        const outputValue = filter.getOutputValue();
        const isActive = filter.isActive();
        const isBypassed = filter.isBypassed();

        // Filter body (funnel/filter shape - cyan/blue)
        const bodyColor = isBypassed ? 0x808080 : (isActive ? 0x00CED1 : 0x4682B4);
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-35, -30, 70, 60, 5);
        this.graphics.endFill();

        // Border with state indicator
        const borderColor = isBypassed ? 0xFFFF00 : (isActive ? 0x00FF00 : 0x696969);
        this.graphics.lineStyle(3, borderColor);
        this.graphics.drawRoundedRect(-35, -30, 70, 60, 5);

        // Funnel shape visualization
        this.graphics.lineStyle(0);
        this.graphics.beginFill(0x1E90FF, 0.5);
        // Top wide part (input)
        this.graphics.drawRect(-25, -20, 50, 8);
        // Middle narrowing
        const funnelPath = [
            -20, -12,
            -10, 0,
            -5, 12,
            5, 12,
            10, 0,
            20, -12
        ];
        this.graphics.drawPolygon(funnelPath);
        // Bottom narrow part (output)
        this.graphics.drawRect(-5, 12, 10, 8);
        this.graphics.endFill();

        // Mode indicator
        const modeNames = ['ALL', 'PASS', 'BLOCK', 'XFORM'];
        const modeText = new PIXI.Text(modeNames[mode] || '?', {
            fontSize: 10,
            fill: 0xFFFF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        modeText.anchor.set(0.5);
        modeText.y = -3;
        this.graphics.addChild(modeText);

        // Pass/Block counts
        const statsText = new PIXI.Text(`✓${passedCount} ✗${blockedCount}`, {
            fontSize: 8,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 1,
            fontFamily: 'monospace'
        });
        statsText.anchor.set(0.5);
        statsText.y = 22;
        this.graphics.addChild(statsText);

        // Input/Output indicators (if processing)
        if (passedCount > 0 || blockedCount > 0) {
            // Input arrow (top)
            this.graphics.lineStyle(2, 0x00FF00);
            this.graphics.moveTo(0, -25);
            this.graphics.lineTo(-5, -20);
            this.graphics.moveTo(0, -25);
            this.graphics.lineTo(5, -20);

            // Output arrow (bottom) - color based on last result
            const outputColor = passedCount > blockedCount ? 0x00FF00 : 0xFF0000;
            this.graphics.lineStyle(2, outputColor);
            this.graphics.moveTo(0, 20);
            this.graphics.lineTo(-5, 15);
            this.graphics.moveTo(0, 20);
            this.graphics.lineTo(5, 15);
        }

        // Label
        const label = new PIXI.Text('Filter', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 40;
        this.graphics.addChild(label);

        // State badge (top-left)
        const stateNames = ['IDLE', 'ACTIVE', 'BYPASS'];
        const stateBadge = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 7,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1,
            backgroundColor: borderColor,
            padding: 2
        });
        stateBadge.anchor.set(0, 0);
        stateBadge.x = -33;
        stateBadge.y = -28;
        this.graphics.addChild(stateBadge);
    }

    private drawAccumulator(): void {
        const accumulator = this.wasmSprite as ToonTalkAccumulator;
        const mode = accumulator.getModeInt();
        const state = accumulator.getStateInt();
        const count = accumulator.getCount();
        const currentValue = accumulator.getCurrentValue();
        const sum = accumulator.getSum();
        const minValue = accumulator.getMinValue();
        const maxValue = accumulator.getMaxValue();
        const average = accumulator.getAverage();
        const isAccumulating = accumulator.isAccumulating();

        // Accumulator body (container/bucket shape - green)
        const bodyColor = isAccumulating ? 0x32CD32 : 0x228B22;
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-35, -30, 70, 60, 5);
        this.graphics.endFill();

        // Border with accumulating indicator
        const borderColor = isAccumulating ? 0x00FF00 : 0x006400;
        this.graphics.lineStyle(3, borderColor);
        this.graphics.drawRoundedRect(-35, -30, 70, 60, 5);

        // Container visualization (bucket/accumulator shape)
        this.graphics.lineStyle(0);
        this.graphics.beginFill(0x90EE90, 0.3);
        // Trapezoid shape (wider at top)
        const bucketPath = [
            -25, -15,
            25, -15,
            20, 15,
            -20, 15
        ];
        this.graphics.drawPolygon(bucketPath);
        this.graphics.endFill();

        // Fill level indicator (based on count)
        if (count > 0) {
            const fillHeight = Math.min(count / 10, 1.0) * 28; // Max 28 pixels height
            this.graphics.beginFill(0x00FF00, 0.6);
            const fillBottom = 15;
            const fillTop = fillBottom - fillHeight;
            const topWidth = 25 - (fillTop + 15) * 0.17; // Proportional width
            const bottomWidth = 20;
            const fillPath = [
                -bottomWidth, fillBottom,
                bottomWidth, fillBottom,
                topWidth, fillTop,
                -topWidth, fillTop
            ];
            this.graphics.drawPolygon(fillPath);
            this.graphics.endFill();
        }

        // Mode indicator
        const modeNames = ['SUM', 'AVG', 'MIN', 'MAX', 'CNT'];
        const modeText = new PIXI.Text(modeNames[mode] || '?', {
            fontSize: 10,
            fill: 0xFFFF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        modeText.anchor.set(0.5);
        modeText.y = -20;
        this.graphics.addChild(modeText);

        // Current value display
        const valueText = new PIXI.Text(currentValue.toFixed(1), {
            fontSize: 12,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2,
            fontFamily: 'monospace'
        });
        valueText.anchor.set(0.5);
        valueText.y = 0;
        this.graphics.addChild(valueText);

        // Count display
        const countText = new PIXI.Text(`n=${count}`, {
            fontSize: 8,
            fill: 0xFFFFFF,
            stroke: 0x000000,
            strokeThickness: 1,
            fontFamily: 'monospace'
        });
        countText.anchor.set(0.5);
        countText.y = 18;
        this.graphics.addChild(countText);

        // Accumulation indicator (pulsing plus signs when accumulating)
        if (isAccumulating) {
            const pulse = Math.sin(Date.now() / 300) * 0.3 + 0.7;
            for (let i = 0; i < 3; i++) {
                const angle = (i / 3) * Math.PI * 2 + (Date.now() / 1000);
                const radius = 22;
                const x = Math.cos(angle) * radius;
                const y = Math.sin(angle) * radius - 5;

                const plusText = new PIXI.Text('+', {
                    fontSize: 12,
                    fill: 0xFFFF00,
                    fontWeight: 'bold',
                    stroke: 0x000000,
                    strokeThickness: 2,
                    alpha: pulse
                });
                plusText.anchor.set(0.5);
                plusText.x = x;
                plusText.y = y;
                this.graphics.addChild(plusText);
            }
        }

        // Label
        const label = new PIXI.Text('Accum', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 40;
        this.graphics.addChild(label);

        // State badge
        const stateNames = ['IDLE', 'ACCUM', 'PAUSE'];
        const stateBadge = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 7,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1,
            backgroundColor: borderColor,
            padding: 2
        });
        stateBadge.anchor.set(0, 0);
        stateBadge.x = -33;
        stateBadge.y = -28;
        this.graphics.addChild(stateBadge);
    }

    private drawSequencer(): void {
        const sequencer = this.wasmSprite as ToonTalkSequencer;
        const mode = sequencer.getModeInt();
        const state = sequencer.getStateInt();
        const currentStep = sequencer.getCurrentStep();
        const numSteps = sequencer.getNumSteps();
        const progress = sequencer.getProgress();
        const isRunning = sequencer.isRunning();
        const loopCount = sequencer.getLoopCount();

        // Sequencer body (step counter shape - blue/purple)
        const bodyColor = isRunning ? 0x4169E1 : 0x6A5ACD;
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-35, -30, 70, 60, 5);
        this.graphics.endFill();

        // Border with running indicator
        const borderColor = isRunning ? 0x00FF00 : (state === 2 ? 0xFFFF00 : 0x483D8B);
        this.graphics.lineStyle(3, borderColor);
        this.graphics.drawRoundedRect(-35, -30, 70, 60, 5);

        // Step visualization (dots/circles representing steps)
        const dotRadius = 3;
        const dotsPerRow = Math.min(5, numSteps);
        const rows = Math.ceil(numSteps / dotsPerRow);
        const startX = -30;
        const startY = -20;
        const spacing = 12;

        for (let i = 0; i < numSteps; i++) {
            const row = Math.floor(i / dotsPerRow);
            const col = i % dotsPerRow;
            const x = startX + col * spacing;
            const y = startY + row * 10;

            // Current step is highlighted
            if (i === currentStep) {
                this.graphics.beginFill(0xFFFF00);
                this.graphics.drawCircle(x, y, dotRadius + 2);
                this.graphics.endFill();
            } else if (i < currentStep) {
                // Completed steps
                this.graphics.beginFill(0x00FF00, 0.7);
                this.graphics.drawCircle(x, y, dotRadius);
                this.graphics.endFill();
            } else {
                // Upcoming steps
                this.graphics.beginFill(0x808080, 0.5);
                this.graphics.drawCircle(x, y, dotRadius);
                this.graphics.endFill();
            }
        }

        // Current step display
        const stepText = new PIXI.Text(`${currentStep + 1}/${numSteps}`, {
            fontSize: 12,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2,
            fontFamily: 'monospace'
        });
        stepText.anchor.set(0.5);
        stepText.y = 10;
        this.graphics.addChild(stepText);

        // Mode indicator
        const modeNames = ['LIN', 'LOOP', 'PING', 'RAND'];
        const modeText = new PIXI.Text(modeNames[mode] || '?', {
            fontSize: 8,
            fill: 0xFFFF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1
        });
        modeText.anchor.set(0.5);
        modeText.y = -25;
        this.graphics.addChild(modeText);

        // Label
        const label = new PIXI.Text('Sequ', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 40;
        this.graphics.addChild(label);

        // State badge
        const stateNames = ['IDLE', 'RUN', 'PAUSE', 'DONE'];
        const stateBadge = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 7,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1,
            backgroundColor: borderColor,
            padding: 2
        });
        stateBadge.anchor.set(0, 0);
        stateBadge.x = -33;
        stateBadge.y = -28;
        this.graphics.addChild(stateBadge);

        // Loop count indicator
        if (loopCount > 0) {
            const loopText = new PIXI.Text(`↻${loopCount}`, {
                fontSize: 7,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 1,
                backgroundColor: 0x9370DB,
                padding: 2
            });
            loopText.anchor.set(1, 0);
            loopText.x = 33;
            loopText.y = -28;
            this.graphics.addChild(loopText);
        }
    }

    private drawTrigger(): void {
        const trigger = this.wasmSprite as ToonTalkTrigger;
        const mode = trigger.getModeInt();
        const state = trigger.getStateInt();
        const threshold = trigger.getThreshold();
        const currentValue = trigger.getCurrentValue();
        const triggerCount = trigger.getTriggerCount();
        const isArmed = trigger.isArmed();
        const isTriggered = trigger.isTriggered();
        const isLatched = trigger.isLatched();

        // Trigger body (lightning bolt shape - yellow/red)
        const bodyColor = isLatched ? 0xFF4500 : (isTriggered ? 0xFFD700 : (isArmed ? 0xFFA500 : 0x808080));
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-30, -30, 60, 60, 5);
        this.graphics.endFill();

        // Border with state indicator
        const borderColor = isLatched ? 0xFF0000 : (isTriggered ? 0xFFFF00 : (isArmed ? 0x00FF00 : 0x696969));
        this.graphics.lineStyle(3, borderColor);
        this.graphics.drawRoundedRect(-30, -30, 60, 60, 5);

        // Lightning bolt icon
        if (isTriggered || isLatched) {
            this.graphics.lineStyle(0);
            this.graphics.beginFill(0xFFFF00);
            const boltPath = [
                0, -18,
                -8, -5,
                -3, -5,
                -10, 10,
                2, -2,
                -3, -2,
                8, -18
            ];
            this.graphics.drawPolygon(boltPath);
            this.graphics.endFill();
        }

        // Threshold indicator
        const thresholdBar = new PIXI.Graphics();
        thresholdBar.beginFill(0x404040, 0.5);
        thresholdBar.drawRect(-20, 5, 40, 8);
        thresholdBar.endFill();

        // Current value relative to threshold
        const valuePosition = Math.min(Math.max(currentValue / (threshold * 2), 0), 1);
        thresholdBar.beginFill(currentValue >= threshold ? 0x00FF00 : 0xFF6347);
        thresholdBar.drawRect(-20, 5, 40 * valuePosition, 8);
        thresholdBar.endFill();

        // Threshold mark
        const thresholdMark = threshold / (threshold * 2);
        thresholdBar.lineStyle(2, 0xFFFFFF);
        thresholdBar.moveTo(-20 + 40 * thresholdMark, 5);
        thresholdBar.lineTo(-20 + 40 * thresholdMark, 13);
        this.graphics.addChild(thresholdBar);

        // Mode indicator
        const modeNames = ['↑', '↓', '↕', 'H', 'L'];
        const modeText = new PIXI.Text(modeNames[mode] || '?', {
            fontSize: 14,
            fill: 0x000000,
            fontWeight: 'bold'
        });
        modeText.anchor.set(0.5);
        modeText.y = -10;
        this.graphics.addChild(modeText);

        // Trigger count
        if (triggerCount > 0) {
            const countText = new PIXI.Text(`×${triggerCount}`, {
                fontSize: 8,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 1,
                backgroundColor: 0xFF6347,
                padding: 2
            });
            countText.anchor.set(1, 0);
            countText.x = 28;
            countText.y = -28;
            this.graphics.addChild(countText);
        }

        // Label
        const label = new PIXI.Text('Trigger', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 40;
        this.graphics.addChild(label);

        // State badge
        const stateNames = ['IDLE', 'ARMED', 'TRIG', 'LOCK'];
        const stateBadge = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 7,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1,
            backgroundColor: borderColor,
            padding: 2
        });
        stateBadge.anchor.set(0, 0);
        stateBadge.x = -28;
        stateBadge.y = -28;
        this.graphics.addChild(stateBadge);
    }

    private drawScheduler(): void {
        const scheduler = this.wasmSprite as ToonTalkScheduler;
        const mode = scheduler.getModeInt();
        const state = scheduler.getStateInt();
        const delay = scheduler.getDelay();
        const interval = scheduler.getInterval();
        const elapsedTime = scheduler.getElapsedTime();
        const executionCount = scheduler.getExecutionCount();
        const progress = scheduler.getProgress();
        const isWaiting = scheduler.isWaiting();
        const isReady = scheduler.isReady();

        // Scheduler body (clock/timer shape - green/teal)
        const bodyColor = isWaiting ? 0x20B2AA : (isReady ? 0x00FF00 : 0x2E8B57);
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-35, -30, 70, 60, 5);
        this.graphics.endFill();

        // Border with state indicator
        const borderColor = isReady ? 0xFFFF00 : (isWaiting ? 0x00FF00 : 0x006400);
        this.graphics.lineStyle(3, borderColor);
        this.graphics.drawRoundedRect(-35, -30, 70, 60, 5);

        // Clock face
        this.graphics.lineStyle(2, 0x000000);
        this.graphics.beginFill(0xFFFFFF, 0.8);
        this.graphics.drawCircle(0, -5, 18);
        this.graphics.endFill();

        // Clock hand (shows progress)
        const handAngle = -Math.PI / 2 + (progress * Math.PI * 2);
        this.graphics.lineStyle(3, 0xFF0000);
        this.graphics.moveTo(0, -5);
        this.graphics.lineTo(Math.cos(handAngle) * 15, -5 + Math.sin(handAngle) * 15);

        // Center dot
        this.graphics.lineStyle(0);
        this.graphics.beginFill(0x000000);
        this.graphics.drawCircle(0, -5, 2);
        this.graphics.endFill();

        // Elapsed time display
        const timeText = new PIXI.Text(`${elapsedTime.toFixed(1)}s`, {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2,
            fontFamily: 'monospace'
        });
        timeText.anchor.set(0.5);
        timeText.y = 15;
        this.graphics.addChild(timeText);

        // Mode indicator
        const modeNames = ['ONCE', 'INTV', 'CRON'];
        const modeText = new PIXI.Text(modeNames[mode] || '?', {
            fontSize: 8,
            fill: 0xFFFF00,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1
        });
        modeText.anchor.set(0.5);
        modeText.y = -27;
        this.graphics.addChild(modeText);

        // Execution count
        if (executionCount > 0) {
            const countText = new PIXI.Text(`×${executionCount}`, {
                fontSize: 8,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 1,
                backgroundColor: 0x9370DB,
                padding: 2
            });
            countText.anchor.set(1, 0);
            countText.x = 33;
            countText.y = -28;
            this.graphics.addChild(countText);
        }

        // Label
        const label = new PIXI.Text('Sched', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 40;
        this.graphics.addChild(label);

        // State badge
        const stateNames = ['IDLE', 'WAIT', 'READY', 'EXEC'];
        const stateBadge = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 7,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 1,
            backgroundColor: borderColor,
            padding: 2
        });
        stateBadge.anchor.set(0, 0);
        stateBadge.x = -33;
        stateBadge.y = -28;
        this.graphics.addChild(stateBadge);

        // Ready indicator (pulsing when ready)
        if (isReady) {
            const pulse = Math.sin(Date.now() / 200) * 0.5 + 0.5;
            this.graphics.beginFill(0xFF0000, 0.5 + pulse * 0.5);
            this.graphics.drawCircle(28, -23, 4);
            this.graphics.endFill();
        }
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

    private drawExpander(): void {
        const expander = this.wasmSprite as ToonTalkExpander;
        const state = expander.getStateInt();
        const scale = expander.getScaleFactor();
        const targetScale = expander.getTargetScale();

        // Bike pump body (cylindrical with handle)
        const bodyColor = state < 3 ? 0xFF6347 : 0x4682B4; // Red for expand, blue for shrink

        // Pump cylinder
        this.graphics.beginFill(bodyColor);
        this.graphics.drawRoundedRect(-15, -30, 30, 60, 5);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(2, 0x000000);
        this.graphics.drawRoundedRect(-15, -30, 30, 60, 5);

        // Handle (moves based on scale)
        const handleY = -40 - (scale * 10);
        this.graphics.lineStyle(4, 0x8B4513);
        this.graphics.moveTo(-20, handleY);
        this.graphics.lineTo(20, handleY);

        // Piston rod
        this.graphics.lineStyle(3, 0x696969);
        this.graphics.moveTo(0, handleY);
        this.graphics.lineTo(0, -30);

        // Direction arrows
        if (state < 3) { // Expanding
            // Up arrows
            for (let i = 0; i < 3; i++) {
                const arrowY = -10 + i * 15;
                this.graphics.lineStyle(2, 0xFFFF00);
                this.graphics.moveTo(-10, arrowY);
                this.graphics.lineTo(0, arrowY - 10);
                this.graphics.lineTo(10, arrowY);
            }
        } else if (state < 6) { // Shrinking
            // Down arrows
            for (let i = 0; i < 3; i++) {
                const arrowY = -10 + i * 15;
                this.graphics.lineStyle(2, 0x00FFFF);
                this.graphics.moveTo(-10, arrowY - 10);
                this.graphics.lineTo(0, arrowY);
                this.graphics.lineTo(10, arrowY - 10);
            }
        }

        // Scale display
        const scaleText = new PIXI.Text(`×${scale.toFixed(2)}`, {
            fontSize: 12,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 3
        });
        scaleText.anchor.set(0.5);
        scaleText.y = 0;
        this.graphics.addChild(scaleText);

        // Label
        const label = new PIXI.Text('Expander', {
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
        const stateNames = ['BIGGER', 'TALLER', 'WIDER', 'SMALLER', 'SHORTER', 'NARROWER', 'GOOD'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: state < 3 ? 0xFF6347 : 0x4682B4,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -45;
        this.graphics.addChild(stateText);
    }

    private drawCopier(): void {
        const copier = this.wasmSprite as ToonTalkCopier;
        const state = copier.getStateInt();
        const copyCount = copier.getCopyCount();
        const hasAttached = copier.hasAttached();

        // Magic wand star tip
        this.graphics.beginFill(0xFFD700); // Gold star
        this.drawStar(0, -25, 5, 12, 6);
        this.graphics.endFill();

        // Sparkles around star when active
        if (hasAttached) {
            for (let i = 0; i < 4; i++) {
                const angle = (i / 4) * Math.PI * 2;
                const distance = 20;
                const x = Math.cos(angle) * distance;
                const y = -25 + Math.sin(angle) * distance;
                this.graphics.beginFill(0xFFFFFF, 0.7);
                this.drawStar(x, y, 4, 5, 2.5);
                this.graphics.endFill();
            }
        }

        // Wand handle
        this.graphics.beginFill(0x8B4513); // Brown
        this.graphics.drawRect(-3, -20, 6, 40);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(2, 0x000000);
        this.graphics.drawRect(-3, -20, 6, 40);

        // Copy icon
        if (hasAttached) {
            const icon = new PIXI.Text('📋', {
                fontSize: 20
            });
            icon.anchor.set(0.5);
            icon.y = 5;
            this.graphics.addChild(icon);
        }

        // Copy counter
        if (copyCount > 0) {
            const counterText = new PIXI.Text(`${copyCount}`, {
                fontSize: 14,
                fill: 0xFFFFFF,
                fontWeight: 'bold',
                stroke: 0x000000,
                strokeThickness: 3,
                backgroundColor: 0xFF6347,
                padding: 2
            });
            counterText.anchor.set(0.5);
            counterText.x = 20;
            counterText.y = -25;
            this.graphics.addChild(counterText);
        }

        // Label
        const label = new PIXI.Text('Copier', {
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
        const stateNames = ['NORMAL', 'ORIGINAL', 'SELF'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: 0xFFD700,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -40;
        this.graphics.addChild(stateText);
    }

    private drawEraser(): void {
        const eraser = this.wasmSprite as ToonTalkEraser;
        const state = eraser.getStateInt();
        const erasedCount = eraser.getErasedCount();
        const progress = eraser.getEraseProgress();

        // Eraser body (pink eraser shape)
        this.graphics.beginFill(state === 1 ? 0xFF69B4 : 0xFFC0CB); // Brighter when erasing
        this.graphics.drawRoundedRect(-20, -25, 40, 50, 8);
        this.graphics.endFill();

        // Border
        this.graphics.lineStyle(2, 0x000000);
        this.graphics.drawRoundedRect(-20, -25, 40, 50, 8);

        // Eraser stripes (texture)
        this.graphics.lineStyle(1, 0xFFB6C1);
        for (let i = 0; i < 3; i++) {
            const y = -15 + i * 12;
            this.graphics.moveTo(-15, y);
            this.graphics.lineTo(15, y);
        }

        // Erasing effect (particles)
        if (state === 1) {
            for (let i = 0; i < 8; i++) {
                const angle = (i / 8) * Math.PI * 2 + Date.now() / 200;
                const distance = 25 + progress * 10;
                const x = Math.cos(angle) * distance;
                const y = Math.sin(angle) * distance;
                const alpha = 1 - progress;
                this.graphics.beginFill(0xFFFFFF, alpha);
                this.graphics.drawCircle(x, y, 3);
                this.graphics.endFill();
            }
        }

        // Progress bar (when erasing)
        if (state === 1) {
            this.graphics.lineStyle(0);
            this.graphics.beginFill(0xFF0000, 0.3);
            this.graphics.drawRect(-20, -30, 40, 4);
            this.graphics.endFill();
            this.graphics.beginFill(0x00FF00);
            this.graphics.drawRect(-20, -30, 40 * progress, 4);
            this.graphics.endFill();
        }

        // Erased count
        if (erasedCount > 0) {
            const counterText = new PIXI.Text(`${erasedCount}`, {
                fontSize: 16,
                fill: 0xFF0000,
                fontWeight: 'bold',
                stroke: 0xFFFFFF,
                strokeThickness: 3
            });
            counterText.anchor.set(0.5);
            counterText.y = 0;
            this.graphics.addChild(counterText);
        }

        // Label
        const label = new PIXI.Text('Eraser', {
            fontSize: 10,
            fill: 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        label.anchor.set(0.5);
        label.y = 35;
        this.graphics.addChild(label);

        // State text
        const stateNames = ['READY', 'ERASING', 'DONE'];
        const stateText = new PIXI.Text(stateNames[state] || '?', {
            fontSize: 8,
            fill: state === 1 ? 0xFF0000 : 0xFFFFFF,
            fontWeight: 'bold',
            stroke: 0x000000,
            strokeThickness: 2
        });
        stateText.anchor.set(0.5);
        stateText.y = -38;
        this.graphics.addChild(stateText);
    }

    /**
     * Set up mouse interactions
     */
    private setupInteractions(): void {
        // Hover effects
        this.container.on('pointerover', () => {
            if (!this.isDragging) {
                this.isHovered = true;
                this.container.scale.set(1.1);
            }
        });

        this.container.on('pointerout', () => {
            if (!this.isDragging) {
                this.isHovered = false;
                this.container.scale.set(1);
            }
        });

        // Start drag
        this.container.on('pointerdown', (event: PIXI.FederatedPointerEvent) => {
            this.startDrag(event);
        });
    }

    /**
     * Start dragging the sprite
     */
    private startDrag(event: PIXI.FederatedPointerEvent): void {
        this.isDragging = true;
        this.originalScale = { x: this.container.scale.x, y: this.container.scale.y };

        // Calculate drag offset (where on the sprite did we click?)
        const spritePos = this.container.position;
        this.dragOffset.x = spritePos.x - event.global.x;
        this.dragOffset.y = spritePos.y - event.global.y;

        // Visual feedback - lift the sprite
        this.container.scale.set(1.15);
        this.container.alpha = 0.8;

        // Move to front (higher z-index)
        if (this.container.parent) {
            this.container.parent.setChildIndex(this.container, this.container.parent.children.length - 1);
        }

        // Set up global move and up handlers
        const onPointerMove = (e: PIXI.FederatedPointerEvent) => {
            this.onDrag(e);
        };

        const onPointerUp = () => {
            this.stopDrag();
            this.container.off('globalpointermove', onPointerMove);
            this.container.off('pointerup', onPointerUp);
            this.container.off('pointerupoutside', onPointerUp);
        };

        this.container.on('globalpointermove', onPointerMove);
        this.container.on('pointerup', onPointerUp);
        this.container.on('pointerupoutside', onPointerUp);

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

        // Update collision bounds if collision system is available
        const gameEngine = getGameEngine();
        if (gameEngine.isInitialized() && this.collisionId >= 0) {
            try {
                gameEngine.updateCollider(
                    this.collisionId,
                    newX,
                    newY,
                    this.wasmSprite.getWidth(),
                    this.wasmSprite.getHeight()
                );
            } catch (e) {
                // Collision system not available
            }
        }

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
            this.container.scale.set(1.1);
        } else {
            this.container.scale.set(1);
        }
        this.container.alpha = 1.0;

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

        // Sync position from WASM to PixiJS
        this.container.x = this.wasmSprite.getX();
        this.container.y = this.wasmSprite.getY();

        // Sync rotation
        this.container.rotation = this.wasmSprite.getRotation();

        // Sync visibility
        this.container.visible = this.wasmSprite.isVisible();

        // Apply drop animation if active
        const gameEngine = getGameEngine();
        if (gameEngine.isInitialized() && this.dropAnimationId >= 0) {
            try {
                if (gameEngine.isAnimationActive(this.dropAnimationId)) {
                    const animValue = gameEngine.getAnimationValue(this.dropAnimationId);
                    this.container.scale.set(animValue);
                } else {
                    // Animation finished, reset scale
                    this.container.scale.set(1.0);
                    this.dropAnimationId = -1;
                }
            } catch (e) {
                // Animation system not available
            }
        }

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
     * Get the PixiJS container (for highlighting and other operations)
     */
    getGraphics(): PIXI.Container {
        return this.container;
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

            // Animate the drop effect
            this.animateDropEffect();

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

            // Animate the drop effect
            this.animateDropEffect();

            // Remove the dropped text
            this.renderer.removeWasmSprite(droppedSprite);
        }

        // Box accepts anything
        else if (myType === 'box') {
            const box = this.wasmSprite as ToonTalkBox;

            if (!box.isFull()) {
                box.addItem();
                console.log(`✨ Added item to box (${box.getCount()} items)`);

                // Animate the drop effect
                this.animateDropEffect();

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

                        // Animate the drop effect
                        this.animateDropEffect();

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
     * Animate a smooth drop effect (like original ToonTalk!)
     * Makes the target sprite bounce when something is dropped on it
     */
    private animateDropEffect(): void {
        const gameEngine = getGameEngine();
        if (!gameEngine.isInitialized() || !gameEngine.getAnimator()) return;

        try {
            // Create a bounce animation on the scale
            const scaleStart = 1.0;
            const scaleEnd = 1.3;
            const duration = 0.3; // 300ms

            // Bounce effect using EASE_OUT_BOUNCE easing
            const animId = gameEngine.createAnimation(
                scaleStart,
                scaleEnd,
                duration,
                10 // EASE_OUT_BOUNCE = 10
            );

            this.dropAnimationId = animId;

            // Apply animation in update loop
            console.log('[Animation] Started drop bounce effect');
        } catch (e) {
            // Animation system not available
        }
    }

    /**
     * Clean up resources
     */
    destroy(): void {
        if (this.destroyed) return;

        this.destroyed = true;

        // Unregister from game engine systems
        const gameEngine = getGameEngine();
        if (gameEngine.isInitialized()) {
            try {
                if (this.entityId >= 0) {
                    gameEngine.destroyEntity(this.entityId);
                }
                // Note: CollisionManager doesn't have unregister yet,
                // it will be cleaned when objects are removed
            } catch (e) {
                // Game engine systems not available
            }
        }

        this.container.destroy({ children: true }); // Destroys container and all children (sprite, graphics, text)
        this.wasmSprite.delete(); // Important: free C++ memory!

        console.log('[WasmSpriteView] Destroyed');
    }
}
