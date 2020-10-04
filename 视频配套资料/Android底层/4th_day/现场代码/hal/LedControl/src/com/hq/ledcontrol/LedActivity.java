package com.hq.ledcontrol;

import com.hq.bean.Girl;
import com.hq.lowlevel.LedNative;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;


public class LedActivity extends Activity implements OnClickListener{
	final String TAG = "LedActivity";
	Button btn_led_on;
	Button btn_led_off;
	TextView  textView1;
	
	LedNative myled;
	
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_led);
        
        initUI();
        
        // 3--调用本地方法
        myled = new LedNative();
        
        myled.openDev();
        
        //获取数据
        int result = myled.addNum(30, 8);
        String str = myled.getJniString();
        Girl g = myled.getJniGirl();
        
        //显示
        textView1.setText("a + b =" + result + "\n"
        		+"str = " + str + "\n"
        		+"name = " + g.getName() + "\n"
        		+"age = " + g.getAge() + "\n"
        		+"heigth = " + g.getHeight() + "\n"
        		);
        
        //给出数据
        myled.setJniString("^_^ hello from java");
        
        Girl g2 = new Girl();
        g2.setName("marong");
        g2.setAge(30);
        g2.setHeight(1.2);
        myled.setJniGirl(g2);
        
    }
    

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch (v.getId()) {
		case R.id.btn_led_on:
			Toast.makeText(LedActivity.this, "亮灯拉----", 1000).show();
			Log.d(TAG, "亮瞎了狗眼");
			
			myled.devOn();
			
			break;
		case R.id.btn_led_off:
			Toast.makeText(LedActivity.this, "灭灯拉----", 1000).show();
			Log.d(TAG, "好黑,好冷,好寂寞");
			
			myled.devOff();
			
			break;

		default:
			break;
		}
		
		
	}

	private void initUI() {
		// TODO Auto-generated method stub
		
		btn_led_on = (Button) findViewById(R.id.btn_led_on);
		btn_led_on.setOnClickListener(this);
		
		btn_led_off = (Button) findViewById(R.id.btn_led_off);
		btn_led_off.setOnClickListener(this);
		
		textView1 = (TextView) findViewById(R.id.textView1);
	}


	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		myled.closeDev();
	}
    
}
