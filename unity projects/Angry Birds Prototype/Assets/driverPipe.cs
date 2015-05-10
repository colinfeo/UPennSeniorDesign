using UnityEngine;
using System;
using System.Collections;
using System.IO;
using System.Net.Sockets;
using NetMQ;

public class driverPipe : MonoBehaviour {
	NetMQSocket client;
	public float[] forceFloats;
	float timeToSend;
	float delay; //ms between sends
	// Use this for initialization
	void Start () {
		forceFloats = new float[2];
		forceFloats [0] = 0.0f; 
		forceFloats [1] = 0.0f;
		NetMQContext ctx = NetMQContext.Create ();
		client = ctx.CreatePublisherSocket ();
		client.Options.SendHighWatermark = 2;
		client.Connect("tcp://127.0.0.1:11000"); 	
		timeToSend = Time.fixedTime + delay;
		delay = 0.01f;
	}

	void Update () {
		//byte[] byteArray = new byte[forceFloats.Length * 4];
		//Buffer.BlockCopy (forceFloats, 0, byteArray, 0, byteArray.Length);
		//client.Send(byteArray);
	}
	// Update is called once per frame
	void FixedUpdate () {
		if (Time.fixedTime > timeToSend) {
			byte[] byteArray = new byte[forceFloats.Length * 4];
			Buffer.BlockCopy (forceFloats, 0, byteArray, 0, byteArray.Length);
			client.Send (byteArray);
			//print (forceFloats [0] + " " + forceFloats [1]);
			timeToSend = Time.fixedTime + delay;
		} else {
			//print ("skip");
		}
		//client.Send("Wassup",false,false);
		//string s = client.ReceiveString ();
		//print (s);
	}
	
}
