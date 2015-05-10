using UnityEngine;
using System.Collections;

public class scoreCube : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnCollisionEnter(Collision collision) {
		GameObject main = GameObject.Find ("MainBehavior");
		mainLoop script = main.GetComponent<mainLoop> ();
		script.score += 5;
		DestroyObject (gameObject);
	}
}
