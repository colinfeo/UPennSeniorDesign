using UnityEngine;
using System.Collections;


public class ballLoop : MonoBehaviour {
	// Use this for initialization
	Rigidbody rb;
	LineRenderer rubberband;
	bool following;
	Vector3 startingPosition; //to be used to know where to throw from
	public bool landed;
	GameObject driverPipeObj;
	float deathTime;
	void Start () {

		rb = GetComponent<Rigidbody>();
		following = false;
		landed = false;
		rubberband = GetComponent<LineRenderer> ();
		rubberband.enabled = false;
		driverPipeObj = GameObject.Find ("driverPipeObject");
		deathTime = Mathf.Infinity;
	}
	
	// Update is called once per frame
	void Update () {

	}

	void OnCollisionEnter(Collision collisionInfo) {
		print (collisionInfo.collider.name);
		//if(collisionInfo.collider.tag.Equals("Ground Plane")) {
		//	landed = true;
		//}

	}

	void FixedUpdate() {
		if(following) {
			rubberband.enabled=true;
			Ray worldPoint = Camera.main.ScreenPointToRay(Input.mousePosition);
			transform.position = worldPoint.GetPoint(10f);
			rubberband.SetPosition (1, transform.position);


			Vector3 releaseVector = 2.0f * (startingPosition - transform.position) - (1.0f*(rb.velocity.magnitude) * (startingPosition - transform.position));
			driverPipe script = driverPipeObj.GetComponent<driverPipe>();
			script.forceFloats[0] = releaseVector[0];
			script.forceFloats[1] = releaseVector[1];
		}
		if (deathTime < Time.fixedTime) {

			DestroyObject(gameObject);
		}
	}

	void OnMouseDown () {

		//Debug.Log("got clicked");
		//rb.AddForce(new Vector3(1000f,0f,0f) );

		following = true;
		startingPosition = transform.position;

	}

	void OnMouseUp () {
		following = false;
		rb.useGravity = true;
		Vector3 releaseVector = startingPosition - transform.position;
		releaseVector.z = 0;
		float multiplier = 1500f;
		rb.AddForce(releaseVector * multiplier * 2.0f);
		rubberband.enabled = false;
		landed = true;
		deathTime = Time.fixedTime + 10f;


		driverPipe script = driverPipeObj.GetComponent<driverPipe>();
		script.forceFloats[0] = 0.0f;
		script.forceFloats[1] = 0.0f;
	}
}
