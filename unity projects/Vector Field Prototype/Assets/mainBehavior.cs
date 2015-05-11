using UnityEngine;
using System.Collections;
using System.Collections.Generic;



public class mainBehavior : MonoBehaviour {
	public GameObject arrow;
	public GameObject chargePoint;
	public GameObject mouseMarker;
	public GameObject driverPipeObj;
	public float num_XArrows, num_YArrows;
	Rigidbody rb;

	// Use this for initialization
	List<GameObject> arrows = new List<GameObject>();
	List<GameObject> charges = new List<GameObject>();
	List<bool> polarities = new List<bool>();
	void Start () {
		num_XArrows = 60;
		num_YArrows = 40;
		for (int i= 0; i < num_XArrows; i++) {
			for( int j = 0; j < num_YArrows; j++ ) {
				//Object.Instantiate(arrow,new Vector3((float)i-10.0f,(float)i-5.0f,0.0f),Quaternion.LookRotation(new Vector3(2.0f,2.0f,0.0f),new Vector3(0.0f,0.0f,0.0f) ) );
				Object newObj = Instantiate(arrow,new Vector3((float)i*(17.0f/num_XArrows)-8.5f,(float)j*(11.0f/num_YArrows)-5.5f,0.0f),Quaternion.Euler(0.0f,0.0f,0.0f) );
				SpriteRenderer sr = ((GameObject)newObj).GetComponent<SpriteRenderer>();
				Color color = sr.material.color;
				color.a = 0.0f;
				sr.material.color = color;
				arrows.Add((GameObject)newObj);
			}
		}
		rb = mouseMarker.GetComponent<Rigidbody> ();
	}
	
	// Update is called once per frame
	void Update () {
		Vector3 mouseScreen = new Vector3(Input.mousePosition.x,Input.mousePosition.y,0.0f);
		Vector3 mouseWorld = Camera.main.ScreenToWorldPoint(mouseScreen);
		mouseWorld[2] = 0.0f;
		mouseMarker.transform.position = mouseWorld;

		if (Input.GetMouseButton (0)) {
			Object newObj = Instantiate(chargePoint,mouseWorld,Quaternion.identity);
			charges.Add((GameObject)newObj);
			polarities.Add(true);
			updateArrows();
		}
		if (Input.GetMouseButton (1)) {
			Object newObj = Instantiate(chargePoint,mouseWorld,Quaternion.identity);
			charges.Add((GameObject)newObj);
			MeshRenderer sr = ((GameObject)newObj).GetComponent<MeshRenderer>();
			Color color = sr.material.color;
			color.r = 255;
			color.g = 0;
			color.b = 0;
			sr.material.color = color;
			polarities.Add(false);
			updateArrows();
		}
		if (Input.GetKeyDown ("space")) {
			for(int j = 0; j < charges.Count;j++) {
				Object.Destroy(charges[j]);
			}
			charges.Clear();
			polarities.Clear();
			updateArrows();
		}

	}

	void FixedUpdate() {
		Vector3 mouseScreen = new Vector3(Input.mousePosition.x,Input.mousePosition.y,0.0f);
		Vector3 mouseWorld = Camera.main.ScreenToWorldPoint(mouseScreen);
		mouseWorld[2] = 0.0f;

		//get force to send to driverPipe
		Vector3 force = getForce(mouseWorld);
		//force += (-0.1f*(rb.velocity.magnitude) * (force.normalized));
		if (force.magnitude > 5.0f) {
			force = (force/force.magnitude)*5.0f;
		}
		if (float.IsNaN(force[0])){
			force[0] = 0.0f;
			force[1] = 0.0f;
		}
		//print (force);
		driverPipe script = driverPipeObj.GetComponent<driverPipe>();
		script.forceFloats[0] = force[0];
		script.forceFloats[1] = force[1];
	}
	
	void updateArrows() {
		//iterate through arrows
		for (int i = 0; i < arrows.Count; i++) {
			//position of current arrow
			Vector3 currPos = arrows[i].transform.position;
			Vector3 forceExperienced = getForce(currPos);
			float angle = Mathf.Atan2(forceExperienced.y, forceExperienced.x) * Mathf.Rad2Deg;
			arrows[i].transform.rotation = Quaternion.AngleAxis(angle, Vector3.forward);
			float magnitude = forceExperienced.magnitude;
			magnitude /= 4.0f;
			//if (magnitude>0.99f){
			//	magnitude = 0.99f;
			//}
			SpriteRenderer sr = arrows[i].GetComponent<SpriteRenderer>();
			Color color = sr.material.color;
			color.a = magnitude;
			sr.material.color = color;
		}

	}

	//calculates sum of forces at position and returns as vector
	Vector3 getForce(Vector3 position) {
		Vector3 returnForce = new Vector3 (0.0f, 0.0f, 0.0f);
		if (charges.Count == 0) {
			return returnForce;
		}
		for(int j = 0; j < charges.Count; j++) {
			Vector3 chargePos = charges[j].transform.position;
			float pol = 1.0f;
			if(!polarities[j]) {
				pol = -1.0f; 
			}
			returnForce += pol * ((position - chargePos)/(Mathf.Pow(Vector3.Distance(chargePos,position),2)));
		}
		return returnForce;
	}

}
