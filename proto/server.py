from concurrent import futures
import time

import grpc
import highway_pb2
import highway_pb2_grpc

class HighwaySenderServicer(highway_pb2_grpc.HighwaySenderServicer):

    def SendHighway(self, request, context):

        self.deal_with_message(request.data)

        reply = highway_pb2.Reply()
        reply.message = f"{request.data}"

        return reply


    # CHANGE THIS
    def deal_with_message(self, message_data):
        print(f"A mensagem passada foi {message_data}")


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    highway_pb2_grpc.add_HighwaySenderServicer_to_server(HighwaySenderServicer(), server)
    server.add_insecure_port("localhost:50051")
    server.start()
    server.wait_for_termination()

if __name__ == "__main__":
    serve()