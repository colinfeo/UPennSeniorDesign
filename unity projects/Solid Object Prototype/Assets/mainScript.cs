using UnityEngine;
using System.Collections;

public class mainScript : MonoBehaviour {
	driverPipe dpipeScript;
	public GameObject bigSphere;
	Rigidbody rb;
	// Use this for initialization
	void Start () {
		GameObject dpipe = GameObject.Find ("driverPipe");
		dpipeScript = dpipe.GetComponent<driverPipe> ();
		rb = bigSphere.GetComponent<Rigidbody>();
	}
	
	// Update is called once per frame
	void Update () {
	
	}
	void FixedUpdate() {
		Vector3 mouseScreen = new Vector3(Input.mousePosition.x,Input.mousePosition.y,0.0f);
		Vector3 mouseWorld = Camera.main.ScreenToWorldPoint(mouseScreen);
		mouseWorld[2] = 0.0f;
		//print (mouseWorld);
		Vector3 vectorToMouse = mouseWorld - bigSphere.transform.position;
		//print (vectorToMouse);
		//if colliding
		if (vectorToMouse.magnitude <= 2.0f) {
			float springForces = (2.0f - vectorToMouse.magnitude);
			Vector3 sendForce = (vectorToMouse.normalized) * springForces * 200.0f;
			dpipeScript.forceFloats [0] = sendForce [0];
			dpipeScript.forceFloats [1] = sendForce [1];
			rb.AddForce(-3000.0f * vectorToMouse * springForces);
			//dpipeScript.sendNow();
		} else {
			dpipeScript.forceFloats [0] = 0.0f;
			dpipeScript.forceFloats [1] = 0.0f;
			//dpipeScript.sendNow();
		}
		//rb.AddForce (new Vector3(0.0f,-50.0f,0.0f));
	}
}
