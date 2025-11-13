import { defineConfig } from 'vite';
import path from 'path';

export default defineConfig({
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
      '@core': path.resolve(__dirname, './core/build')
    }
  },
  server: {
    port: 3000,
    open: true,
    fs: {
      // Allow serving files from one level up (for accessing core/build)
      allow: ['..']
    }
  },
  build: {
    target: 'esnext',
    outDir: 'dist',
    assetsDir: 'assets',
    sourcemap: true
  },
  optimizeDeps: {
    exclude: ['toontalk-core']
  },
  cacheDir: 'node_modules/.vite-cache'  // Use different cache dir to avoid locks
});
