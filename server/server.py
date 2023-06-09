from concurrent import futures
import grpc
import highway_pb2
import highway_pb2_grpc

import redis

# Set the IP address of the server + port
IP_TO_SENDER = "192.168.0.74:50051"

class HighwaySenderServicer(highway_pb2_grpc.HighwaySenderServicer):

    def SendHighway(self, request, context):

        self.deal_with_message(request.data)

        reply = highway_pb2.Reply()
        reply.message = f"{request.data}"

        return reply

    def deal_with_message(self, message_data):
        r = redis.Redis(host='localhost', port=6379, db=0)

        key, message = message_data.split("$")

        r.set(key, message)

def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    highway_pb2_grpc.add_HighwaySenderServicer_to_server(HighwaySenderServicer(), server)
    server.add_insecure_port(IP_TO_SENDER)
    server.start()
    server.wait_for_termination()

if __name__ == "__main__":
    serve()
    