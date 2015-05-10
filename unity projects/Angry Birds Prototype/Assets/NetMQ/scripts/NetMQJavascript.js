#pragma strict
import NetMQ;

function Start () {
		var ctx = NetMQContext.Create();
		var server = ctx.CreateResponseSocket();
		server.Bind("tcp://127.0.0.1:5556");
		var client = ctx.CreateRequestSocket();
		client.Connect("tcp://127.0.0.1:5556");
		client.Send("Hello", false, false); 
		var m1 = server.ReceiveString();
		Debug.Log("From Client: " + m1);
		server.Send("Hi Back", false, false);
		var m2 = client.ReceiveString();
		Debug.Log("From Server: " + m2);
		client.Disconnect("tcp://127.0.0.1:5556");
		server.Unbind("tcp://127.0.0.1:5556");
}

function Update () {

}