import UnityEngine
import NetMQ

class NetMQBoo (MonoBehaviour): 

	def Start ():
        ctx as NetMQContext = NetMQContext.Create()
        server as NetMQSocket = ctx.CreateResponseSocket()
        server.Bind("tcp://127.0.0.1:5556")
        client as NetMQSocket = ctx.CreateRequestSocket()
        client.Connect("tcp://127.0.0.1:5556")
        client.Send("Hello", false, false)
        m1 as string = server.ReceiveString()
        Debug.Log("From Client: " + m1)
        server.Send("Hi Back", false, false)
        m2 as string = client.ReceiveString()
        Debug.Log("From Server: " + m2)
        client.Disconnect("tcp://127.0.0.1:5556")
        server.Unbind("tcp://127.0.0.1:5556")

	def Update ():
		pass