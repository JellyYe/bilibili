package com.hq.json;

import org.json.JSONException;
import org.json.JSONObject;

public class LedJson {
	public final static String LED1_KEY  = "led1";
	public final static String LED2_KEY  = "led1";
	public final static String ALLLED_KEY  = "allled";
	
	
	// {"led1":true, "led2":true, "allled":true}
	//¹¹½¨json×Ö·û´®
	public String getLedJsonString(boolean led1Value, boolean led2Value, boolean allledValue) throws JSONException{
		
		JSONObject ledjsonobj = new JSONObject();
		
		ledjsonobj.put(LED1_KEY, led1Value);
		ledjsonobj.put(LED2_KEY, led2Value);
		ledjsonobj.put(ALLLED_KEY, allledValue);
		
		return ledjsonobj.toString();
	}
	
	//·ÖÎöjson×Ö·û´®
	public boolean getLedStatus(String key, String jsonStr) throws JSONException{
		
		JSONObject obj = new JSONObject(jsonStr);

		return obj.getBoolean(key);
	}
	
}
