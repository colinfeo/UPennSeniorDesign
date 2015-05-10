using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class mainLoop : MonoBehaviour {

	public GameObject gameBall;
	Vector3 startingPoint;
	// Use this for initialization
	Object currentBall;
	Quaternion startingRotation;
	public GameObject targetCube;
	public int score;
	public Text scoreText;
	float cubeSpawnTime; 
	void Start () {
		startingPoint = new Vector3 (-7.75f, 0.0f, 0.0f);
		startingRotation = new Quaternion();
		currentBall = Instantiate(gameBall,startingPoint,startingRotation);
		score = 0;
		//scoreText = new Text ();
		//scoreText.text = score.ToString ();
		cubeSpawnTime = Time.fixedTime + 3.0f;
	}
	
	// Update is called once per frame
	void Update () {
		GameObject currentBallGame = (GameObject)currentBall;
		if (currentBallGame.GetComponent<ballLoop> ().landed == true) {
			currentBall = Instantiate(gameBall,startingPoint,startingRotation);			
		}
		scoreText.text = score.ToString ();
	}

	void FixedUpdate() {
		if (Time.fixedTime > cubeSpawnTime) {
			float randX = Random.value * 3.8f;
			float randY = (Random.value * 7.5f) - 2.5f;
			Instantiate(targetCube,new Vector3(randX,randY,0.0f),Quaternion.identity);
			cubeSpawnTime = Time.fixedTime + 3.0f;
		}
	}
}
