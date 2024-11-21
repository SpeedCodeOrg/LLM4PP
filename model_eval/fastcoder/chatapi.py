from openai import OpenAI
import copy
import uuid

class MessageHistory:
    def __init__(self, parent : 'MessageHistory' = None):
        self.message_history = []
        self.parent = None
        self.parent = parent
        self.children = []
        self.headless_children = []
        if self.parent != None:
            self.message_history = copy.deepcopy(self.parent.message_history)

    # returns a new MessageHistory with the current MessageHistory as the parent. 
    def fork(self):
        self.children.append(MessageHistory(parent = self))
        return self.children[-1]

    def fork_headless(self):
        self.headless_children.append(MessageHistory())
        return self.children[-1]
    
    # returns a copy of the current message history. 
    def get_messages(self):
        return copy.deepcopy(self.message_history)

    # add a message
    def add_message(self, role : str, content : str):
        #if role == "system":
        #    seed = str(uuid.uuid1())
        #    content = f"SEED<{seed}>\n{content}"
        self.message_history.append({"role": role, "content": content})
    
    # reverts to the parent history.
    def revert(self):
        assert(self.parent != None)
        return self.parent
    def dump(self):
        ret = dict()
        ret['message_history'] = self.message_history
        ret['children'] = []
        ret['headless_children'] = []
        for x in self.children:
            ret['children'].append(x.dump())
        for x in self.headless_children:
            ret['headless_children'].append(x.dump())
        return ret

class ChatPIE:
    def __init__(self):
        self.client = OpenAI(api_key="0", base_url="http://queue-g6e2xlarge-dy-g6e2xlarge-1:8000/v1")
        self.prompt_tokens_used = dict()
        self.response_tokens_used = dict()
        self.counter = 0
        self.uuid = uuid.uuid1() 
        pass

    def get_response(self, model, message_history : MessageHistory, json_format = False):
        total_cost = self.get_cost()[2]
        if total_cost > 2.0:
            print("The total cost is too high, quitting")
            quit()
        if model not in self.prompt_tokens_used:
            self.prompt_tokens_used[model] = 0
        if model not in self.response_tokens_used:
            self.response_tokens_used[model] = 0
            
        messages = message_history.get_messages()
        #with open(f'LOG_DIR/{self.uuid}-{self.counter}', 'w+') as log:
        #    for m in messages:
        #        log.write(f"######{m['role']}######\n")
        #        log.write(f"######{m['role']}######\n")
        #        log.write(f"######{m['role']}######\n")
        #        log.write(f"{m['content']}\n")

        if not json_format:
            
            self.counter += 1
            #messages[0]['user_context'] = messages[0]['user']
            #messages[0]['user'] = "You are a GPT that optimizes code. The user will provide you a program and you will produce a complete equivalent program that achieves 10/10 performance."
            #print(messages)
            completion = self.client.chat.completions.create(model=model,
                                                             messages=message_history.get_messages())
        else:
            completion = self.client.chat.completions.create(model=model,
                    messages=message_history.get_messages(), response_format={'type':'json_object'})

        self.response_tokens_used[model] += completion.usage.completion_tokens
        self.prompt_tokens_used[model] += completion.usage.prompt_tokens
        return completion.choices[0].message.content

    def get_usage(self):
        return (self.response_tokens_used, self.prompt_tokens_used)

    def get_cost(self):
        prompt_costs = dict()
        response_costs = dict()
        prompt_costs['gpt-4o'] = 0.005
        response_costs['gpt-4o'] = 0.015
        prompt_costs["gpt-3.5-turbo"] = 0.0005
        response_costs["gpt-3.5-turbo"] = 0.0015
        prompt_costs['gpt-4o-mini'] = 0.000150
        response_costs['gpt-4o-mini'] = 0.000600
        prompt_costs['gpt-4o-2024-08-06'] = 0.00250
        response_costs['gpt-4o-2024-08-06'] = 0.01000

        total_prompt = 0
        total_response = 0
        for x in self.prompt_tokens_used.keys():
            if x not in prompt_costs:
                print(f"Warning {x} not in prompt_costs")
            else:
                total_prompt += (self.prompt_tokens_used[x] * prompt_costs[x]) / 1000

        for x in self.response_tokens_used.keys():
            if x not in response_costs:
                print(f"Warning {x} not in response costs")
            else:
                total_response += (self.response_tokens_used[x] * response_costs[x]) / 1000
        return (total_response, total_prompt, total_prompt+total_response)
        return (self.response_tokens_used, self.prompt_tokens_used)


        
 
class ChatAPI:
    def __init__(self):
        self.client = OpenAI()#api_key="0", base_url="http://queue-g6e2xlarge-dy-g6e2xlarge-1:8000/v1")
        self.prompt_tokens_used = dict()
        self.response_tokens_used = dict()
        self.cached_prompt_tokens = dict()
        self.uuid = uuid.uuid1()
        self.counter = 0
        pass

    def get_response(self, model, message_history : MessageHistory, json_format = False):
        if model not in self.prompt_tokens_used:
            self.prompt_tokens_used[model] = 0
        if model not in self.response_tokens_used:
            self.response_tokens_used[model] = 0
        if model not in self.cached_prompt_tokens:
            self.cached_prompt_tokens[model] = 0
        if not json_format:
            completion = self.client.chat.completions.create(model=model,
                                                             messages=message_history.get_messages())
        else:
            completion = self.client.chat.completions.create(model=model,
                    messages=message_history.get_messages(), response_format={'type':'json_object'})

        #with open(f'LOG_DIR/{self.uuid}-{self.counter}', 'w+') as log:
        #    for m in message_history.get_messages():
        #        log.write(f"######{m['role']}######\n")
        #        log.write(f"######{m['role']}######\n")
        #        log.write(f"######{m['role']}######\n")
        #        log.write(f"{m['content']}\n")
        #    log.write(f"######LAST_ASSISTANT######\n")
        #    log.write(f"######LAST_ASSISTANT######\n")
        #    log.write(f"######LAST_ASSISTANT######\n")
        #    log.write(f"{completion.choices[0].message.content}\n")
        #    log.write(str(self.get_cost())+"\n")
        self.counter += 1


        self.response_tokens_used[model] += completion.usage.completion_tokens
        self.prompt_tokens_used[model] += completion.usage.prompt_tokens
        #print(completion.usage.prompt_tokens_details)
        self.cached_prompt_tokens[model] += completion.usage.prompt_tokens_details.cached_tokens
        return completion.choices[0].message.content

    def get_usage(self):
        return (self.response_tokens_used, self.prompt_tokens_used, self.cached_prompt_tokens)

    def get_cost(self):
        prompt_costs = dict()
        response_costs = dict()
        prompt_costs['gpt-4o'] = 0.005
        response_costs['gpt-4o'] = 0.015
        prompt_costs["gpt-3.5-turbo"] = 0.0005
        response_costs["gpt-3.5-turbo"] = 0.0015
        prompt_costs['gpt-4o-mini'] = 0.000150
        response_costs['gpt-4o-mini'] = 0.000600
        prompt_costs['gpt-4o-2024-08-06'] = 0.00250
        response_costs['gpt-4o-2024-08-06'] = 0.01000

        total_prompt = 0
        total_response = 0
        for x in self.prompt_tokens_used.keys():
            if x not in prompt_costs:
                print(f"Warning {x} not in prompt_costs")
            else:
                total_prompt += (self.prompt_tokens_used[x] * prompt_costs[x]) / 1000

        for x in self.response_tokens_used.keys():
            if x not in response_costs:
                print(f"Warning {x} not in response costs")
            else:
                total_response += (self.response_tokens_used[x] * response_costs[x]) / 1000
        discount = 0
        for x in self.cached_prompt_tokens:
              discount += (self.cached_prompt_tokens[x]*prompt_costs[x])/(2*1000)

        return (total_response, total_prompt, total_prompt+total_response, total_prompt + total_response - discount)
        return (self.response_tokens_used, self.prompt_tokens_used)


        
        


      

    
    
    
    
