package com.hq.bean;

public class Girl {
	private String name;
	private int age;
	private double height;
	
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public int getAge() {
		return age;
	}
	public void setAge(int age) {
		this.age = age;
	}
	public double getHeight() {
		return height;
	}
	public void setHeight(double height) {
		this.height = height;
	}
	
	//������ʾ---jni�ص�java, ����ֵһ����void
	public void setGirlInformation(String name, int age){
		this.name = name;
		this.age = age;
	}
	
	
	
	

}
