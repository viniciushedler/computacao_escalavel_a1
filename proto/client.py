import highway_pb2_grpc
import highway_pb2
import grpc

def send_message(message_data):

    with grpc.insecure_channel('localhost:50051') as channel:
        
        stub = highway_pb2_grpc.HighwaySenderStub(channel)

        highway_request = highway_pb2.HighwayRequest(data = message_data)
        reply = stub.SendHighway(highway_request)

if __name__ == "__main__":
    # area de teste
    send_message(input("Qual a mensagem a passar? "))