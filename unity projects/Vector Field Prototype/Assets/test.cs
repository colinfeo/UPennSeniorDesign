using UnityEngine;
using System.Collections;

public class test : MonoBehaviour {
	float increment;
	// Use this for initialization
	void Start () {
		increment = 0.5f;
	}
	
	// Update is called once per frame
	void Update () {
		this.transform.position = transform.position + new Vector3 (increment, increment, increment);
	}
}
