#include "producing_stage.h"
#include "consuming_stage.h"

namespace ecsms {
    template<typename T>
    class producer_consumer_connection {
    public:
        producer_consumer_connection (std::shared_ptr<producing_stage<T>>, std::shared_ptr<consuming_stage<T>>);
        
    private:
    };
}
