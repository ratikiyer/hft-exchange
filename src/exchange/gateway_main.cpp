#include <grpcpp/grpcpp.h>
#include "proto/exchange.grpc.pb.h"
#include <random>
int main() {
    auto chan = grpc::CreateChannel("localhost:50051",
                                    grpc::InsecureChannelCredentials());
    std::unique_ptr<hft::MatchingGateway::Stub> stub =
        hft::MatchingGateway::NewStub(chan);
    grpc::ClientContext ctx;
    auto stream = stub->Submit(&ctx);

    std::mt19937_64 rng(42);
    std::uniform_int_distribution<uint32_t> px(90,110), qty(1,100);
    std::array<char,16> oid;
    for (int i=0;i<1'000'000;++i) {
        hft::NewOrderSingle o;
        rng(); memcpy(oid.data(), &i, sizeof i);
        o.set_order_id(oid.data(),16);
        o.set_symbol("AAPL");
        o.set_side((i&1)?hft::BUY:hft::SELL);
        o.set_ord_type(hft::LIMIT);
        o.set_price(px(rng));
        o.set_qty(qty(rng));
        o.set_ts(std::chrono::steady_clock::now().time_since_epoch().count());
        stream->Write(o);
        hft::ExecutionReport rpt;
        stream->Read(&rpt);   // blocking; fine for demo
    }
    stream->WritesDone();
    stream->Finish();
}
