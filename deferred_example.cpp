#include <eosiolib/transaction.hpp> // include this for transactions

#include <eosiolib/eosio.hpp>
//#include <eosiolib/eosio.hpp>
//#include <eosiolib/asset.hpp>

class [[eosio::contract("deferred_example")]] deferred_example : public eosio::contract
// Here `deferred_example_name` is the name of your .cpp file where this code is located
{
  public:
    using contract::contract;

    // this action will be called by the deferred transaction
    ACTION deferred(eosio::name from, const std::string &message)
    {
        require_auth(from);
        eosio::print("Printing deferred ", from, message);
    }

    ACTION send(eosio::name from, const std::string &message, uint64_t delay)
    {
        require_auth(from);

        eosio::transaction t{};
        // always double check the action name as it will fail silently
        // in the deferred transaction
        t.actions.emplace_back(
            // when sending to _self a different authorization can be used
            // otherwise _self must be used
            eosio::permission_level(from, "active"_n),
            // account the action should be send to
            _self,
            // action to invoke
            "deferred"_n,
            // arguments for the action
            std::make_tuple(from, message));

        // set delay in seconds
        t.delay_sec = delay;

        // first argument is a unique sender id
        // second argument is account paying for RAM
        // third argument can specify whether an in-flight transaction
        // with this senderId should be replaced
        // if set to false and this senderId already exists
        // this action will fail
        t.send(1, from /*, false */);

        eosio::print("Scheduled with a delay of ", delay);
    }
};

EOSIO_DISPATCH(deferred_example, (send)(deferred))
