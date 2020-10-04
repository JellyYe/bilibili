package com.farsight.activitydemo1;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;


public class MainActivity extends Activity implements OnClickListener{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        Button bt1 = (Button) findViewById(R.id.button1);
        bt1.setOnClickListener(this);
        
        Button bt2 = (Button) findViewById(R.id.button2);
        bt2.setOnClickListener(this);
        
        Button bt3 = (Button) findViewById(R.id.button3);
        bt3.setOnClickListener(this);
        
        Button bt4 = (Button) findViewById(R.id.button4);
        bt4.setOnClickListener(this);
    }

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch (v.getId()) {
		case R.id.button1:

			break;
		case R.id.button2:
				
			//连接服务器
			new ClientThread().start();
			
			break;
			
		
		default:
			break;
		}
	}


}
