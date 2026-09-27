package com.mcpe.client;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.Surface;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public final class MainActivity extends Activity {
    private ClientGLSurfaceView glView;

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().setNavigationBarColor(0xFF101216);
        getWindow().setStatusBarColor(0xFF101216);

        glView = new ClientGLSurfaceView();
        setContentView(glView);
        hideSystemUi();
    }

    private void hideSystemUi() {
        getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_FULLSCREEN |
            View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
            View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
            View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
        );
    }

    @Override protected void onResume() { super.onResume(); glView.onResume(); }
    @Override protected void onPause() { glView.onPause(); super.onPause(); }

    private final class ClientGLSurfaceView extends GLSurfaceView {
        private final RendererImpl renderer;
        private float lastX, lastY;
        private boolean dragging;

        ClientGLSurfaceView() {
            super(MainActivity.this);
            setEGLContextClientVersion(3);
            renderer = new RendererImpl();
            setRenderer(renderer);
            setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
            setFocusable(true);
        }

        @Override public boolean onTouchEvent(MotionEvent e) {
            final float x = e.getX();
            final float y = e.getY();
            final int action = e.getActionMasked();
            switch (action) {
                case MotionEvent.ACTION_DOWN:
                    lastX = x; lastY = y; dragging = true;
                    queueEvent(() -> renderer.nativeTouch(0, x, y, 1));
                    return true;
                case MotionEvent.ACTION_MOVE:
                    lastX = x; lastY = y;
                    queueEvent(() -> renderer.nativeTouch(2, x, y, 1));
                    return true;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    dragging = false;
                    queueEvent(() -> renderer.nativeTouch(1, x, y, 0));
                    return true;
                default:
                    return true;
            }
        }

        private final class RendererImpl implements GLSurfaceView.Renderer {
            private boolean initialized;

            @Override public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                initialized = false;
            }

            @Override public void onSurfaceChanged(GL10 gl, int width, int height) {
                if (!initialized) {
                    nativeInit(width, height);
                    initialized = true;
                } else {
                    nativeResize(width, height);
                }
            }

            @Override public void onDrawFrame(GL10 gl) {
                if (initialized) nativeRender();
            }

            void surfaceDestroyed() {
                if (initialized) {
                    nativeShutdown();
                    initialized = false;
                }
            }

            native void nativeInit(int width, int height);
            native void nativeResize(int width, int height);
            native void nativeRender();
            native void nativeShutdown();
            native void nativeTouch(int action, float x, float y, int down);
        }

        @Override public void surfaceDestroyed(android.view.SurfaceHolder holder) {
            queueEvent(renderer::surfaceDestroyed);
            super.surfaceDestroyed(holder);
        }
    }

    static { System.loadLibrary("mcpe_client"); }
}
