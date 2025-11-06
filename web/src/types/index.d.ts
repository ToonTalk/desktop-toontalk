/**
 * Global type definitions for ToonTalk Web
 */

export * from './wasm';

/**
 * Game object types
 */
export interface GameObject {
    id: string;
    type: string;
    x: number;
    y: number;
    width: number;
    height: number;
    visible: boolean;
}

/**
 * Event types
 */
export interface GameEvent {
    type: string;
    timestamp: number;
    data?: unknown;
}

/**
 * Configuration
 */
export interface GameConfig {
    width: number;
    height: number;
    backgroundColor: number;
    debug: boolean;
}
