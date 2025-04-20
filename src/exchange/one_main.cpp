#include <grpcpp/grpcpp.h>
#include "proto/exchange.grpc.pb.h"
#include "exchange/engine.hpp"
#include "../includes/concurrentqueue.h"

class OmeService final : public hft::MatchingGateway::Service {
public:
    grpc::Status Submit(grpc::ServerContext*,
                        grpc::ServerReaderWriter<hft::ExecutionReport,
                                                 hft::NewOrderSingle>* stream) override
    {
        moodycamel::ConcurrentQueue<hft::ExecutionReport> out_q;
        MatchingEngine eng(out_q);

        hft::NewOrderSingle in;
        while (stream->Read(&in)) {
            eng.on_nos(from_proto(in));
            hft::ExecutionReport rpt;
            while (out_q.try_dequeue(rpt))
                stream->Write(rpt);
        }
        return grpc::Status::OK;
    }
};

int main()
{
    grpc::ServerBuilder b;
    b.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
    OmeService svc; b.RegisterService(&svc);
    std::unique_ptr<grpc::Server> server(b.BuildAndStart());
    server->Wait();
}
