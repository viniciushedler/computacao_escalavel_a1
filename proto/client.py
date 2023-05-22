import highway_pb2_grpc
import highway_pb2
import time
import grpc

def run():
    with grpc.insecure_channel('localhost:50051') as channel:
        stub = highway_pb2_grpc.HighwaySenderStub(channel)

        text = input("Qual a mensagem a passar? ")

        highway_request = highway_pb2.HighwayRequest(data = text)
        reply = stub.SendHighway(highway_request)
        print("SendHighway Response Received:")
        print(reply)
        print("---")



def send_message(message_data):

    message_data = prepare_message(message_data)

    with grpc.insecure_channel('localhost:50051') as channel:
        
        stub = highway_pb2_grpc.HighwaySenderStub(channel)

        highway_request = highway_pb2.HighwayRequest(data = message_data)
        reply = stub.SendHighway(highway_request)


# CHANGE THIS
def prepare_message(message_data):
    message_data += " [FORMATED] "
    return message_data


if __name__ == "__main__":
    # area de teste
    send_message(input("Qual a mensagem a passar? "))