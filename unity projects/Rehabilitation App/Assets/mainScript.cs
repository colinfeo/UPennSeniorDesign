using UnityEngine;
using System.Collections;
using UnityEngine.UI;



public class mainScript : MonoBehaviour {
	//need score
	//need timer
	public GameObject target;
	int time;
	int score;
	public GameObject timeTextObj;
	public GameObject scoreTextObj;
	Text scoreText;
	Text timeText;
	driverPipe dpipeScript;

	bool playing;
	float startTime;
	// Use this for initialization
	void Start () {
		score = 0;
		time = 0;
		playing = false;
		GameObject dpipe = GameObject.Find ("driverPipe");
		dpipeScript = dpipe.GetComponent<driverPipe> ();
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void FixedUpdate () {

		if (Input.GetKey ("space")) {
			//print ("hey!");
			time = 0;
			score = 0;
			playing = true;
			target.transform.position = new Vector3 ((Random.value * 20.0f) - 10.0f, (Random.value * 10.0f) - 5.0f, 0.0f);
			startTime = Time.fixedTime;
			scoreText = scoreTextObj.GetComponent<Text>();
			timeText = timeTextObj.GetComponent<Text>();

		}
		if (time > 30) {
			playing = false;
			dpipeScript.forceFloats[0] = 0.0f;
			dpipeScript.forceFloats[1] = 0.0f;
		}
		Vector3 mouseScreen = new Vector3(Input.mousePosition.x,Input.mousePosition.y,0.0f);
		Vector3 mouseWorld = Camera.main.ScreenToWorldPoint(mouseScreen);
		mouseWorld[2] = 0.0f;

		if (playing) {
			Vector3 vectorToMouse = mouseWorld - target.transform.position;

			if (vectorToMouse.magnitude < 0.25) {
				score ++;
				target.transform.position = new Vector3 ((Random.value * 20.0f) - 10.0f, (Random.value * 10.0f) - 5.0f, 0.0f);
				Vector2 randDirection = Random.insideUnitCircle;
				randDirection = randDirection.normalized;
				randDirection *= 5.0f;
				//randDirection -= new Vector2(4.0f,4.0f);
				dpipeScript.forceFloats[0] = randDirection[0];
				dpipeScript.forceFloats[1] = randDirection[1];
				//add one to score
				//randomly move target
			}
			time = (int)(Time.fixedTime - startTime);
			timeText.text = "TIME: " + time.ToString();
			scoreText.text = "SCORE: " + score.ToString();
		}
		//s1: run for 1 minute
		//s2: place target randomly
		//s3: generate random force
		//s4: when you are within a certain distance add 1 to score
		//s5: if 1 minute is up then reset else goto step 2
	}
}
