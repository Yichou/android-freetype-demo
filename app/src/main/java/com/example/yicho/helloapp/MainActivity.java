package com.example.yicho.helloapp;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;

public class MainActivity extends Activity implements View.OnClickListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("ft");
    }

    Bitmap bitmap;
    MyView mv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.btn).setOnClickListener(this);

        mv = new MyView(this);
        FrameLayout fl = findViewById(R.id.fl);
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        lp.gravity = Gravity.CENTER;
        fl.addView(mv, lp);
    }

    @Override
    public void onClick(View v) {
        stringFromJNI(bitmap);
        mv.invalidate();
    }

    class MyView extends View {
        int vw, vh;

        public MyView(Context context) {
            super(context);

            vw = getWidth();
            vh = getHeight();
            bitmap = Bitmap.createBitmap(320, 480, Bitmap.Config.RGB_565);
        }

        @Override
        protected void onDraw(Canvas canvas) {
            super.onDraw(canvas);

            canvas.drawColor(Color.GRAY);
            canvas.drawBitmap(bitmap, 10, 100, null);
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void stringFromJNI(Bitmap bmp);
}
