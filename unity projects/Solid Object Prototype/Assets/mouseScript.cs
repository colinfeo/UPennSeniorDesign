using UnityEngine;
using System.Collections;

public class mouseScript : MonoBehaviour {
	driverPipe dpipeScript;
	Rigidbody rb;
	bool inObject;
	public GameObject bigBall;
	// Use this for initialization
	void Start () {
		GameObject dpipe = GameObject.Find ("driverPipe");
		dpipeScript = dpipe.GetComponent<driverPipe> ();
		rb = GetComponent<Rigidbody> ();
	}
	
	// Update is called once per frame
	void Update () {

	}
	void FixedUpdate () {
		Vector3 mouseScreen = new Vector3(Input.mousePosition.x,Input.mousePosition.y,0.0f);
		Vector3 mouseWorld = Camera.main.ScreenToWorldPoint(mouseScreen);
		mouseWorld[2] = 0.0f;
		Vector3 vectorToMouse = mouseWorld - rb.position;
		if (vectorToMouse.magnitude > 0.01) {
			rb.velocity = (mouseWorld - rb.position).normalized * 2.50f;
		} else {
			rb.velocity = new Vector3 (0.0f, 0.0f, 0.0f);
		}
	}

	void LateUpdate () {

	}

	void OnCollisionEnter( Collision collision) {
		inObject = true;
		Vector3 boundaryForce = gameObject.transform.position - collision.gameObject.transform.position;
		//boundaryForce = 5.0f * (boundaryForce.normalized);
		float k = 200.0f;
		float distanceFromEdge = 2.4f - (gameObject.transform.position - collision.gameObject.transform.position).magnitude;
		boundaryForce = boundaryForce.normalized * distanceFromEdge * k;
		
		//print (boundaryForce [0], boundaryForce [1]);
		dpipeScript.forceFloats [0] = boundaryForce [0];
		dpipeScript.forceFloats [1] = boundaryForce [1];
		/*
		Vector3 boundaryForce = gameObject.transform.position - collision.gameObject.transform.position;
		//boundaryForce = 5.0f * (boundaryForce.normalized);

		float distanceFromEdge = 2.0f - (gameObject.transform.position - collision.gameObject.transform.position).magnitude;
		boundaryForce = boundaryForce.normalized * distanceFromEdge;

		//print (boundaryForce [0], boundaryForce [1]);
		dpipeScript.forceFloats [0] = boundaryForce [0];
		dpipeScript.forceFloats [1] = boundaryForce [1];
		*/
	}

	void OnCollisionStay (Collision collision) {
		Vector3 boundaryForce = gameObject.transform.position - collision.gameObject.transform.position;
		//boundaryForce = 5.0f * (boundaryForce.normalized);
		float k = 200.0f;
		float distanceFromEdge = 2.4f - (gameObject.transform.position - collision.gameObject.transform.position).magnitude;
		boundaryForce = boundaryForce.normalized * distanceFromEdge * k;
		
		//print (boundaryForce [0], boundaryForce [1]);
		dpipeScript.forceFloats [0] = boundaryForce [0];
		dpipeScript.forceFloats [1] = boundaryForce [1];
		/*
		Vector3 boundaryForce = gameObject.transform.position - collision.gameObject.transform.position;
		boundaryForce = 5.0f * (boundaryForce.normalized);

		//print (boundaryForce [0], boundaryForce [1]);
		dpipeScript.forceFloats [0] = boundaryForce [0];
		dpipeScript.forceFloats [1] = boundaryForce [1];*/
	}

	void OnCollisionExit (Collision collision) {
		GameObject dpipe = GameObject.Find ("driverPipe");
		driverPipe dpipeScript = dpipe.GetComponent<driverPipe> ();
		//print (boundaryForce [0], boundaryForce [1]);
		dpipeScript.forceFloats [0] = 0.0f;
		dpipeScript.forceFloats [1] = 0.0f;
	}
}
