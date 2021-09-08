# -*- coding: utf-8 -*-


from hashlib import sha256 #To hash block in blockchain

def updatehash(*args): #Definition of hash function
    hash_text = ""; h = sha256()
    for arg in args:
        hash_text += str(arg)

    h.update(hash_text.encode('utf-8'))
    return h.hexdigest()



class Block(): #Block class
    data = None #Transactions
    hash = None
    nonce = 0
    previous_hash = "0" * 64

    def __init__(self, data, number = 0):
        self.data = data
        self.number = number

    def hash(self): #Funtion for what will be hashed
        return updatehash(
            self.previous_hash, 
            self.number, 
            self.data, 
            self.nonce
        )

    def __str__(self):
        return str("Block #: %s\nHash: %s\nPrevious: %s\nData: %s\nNonce: %s\n" % (
             self.number, 
             self.hash(), 
             self.previous_hash, 
             self.data, 
             self.nonce
             )
        )



class Blockchain(): #Blockchain class
    difficulty = 4

    def __init__(self,chain=[]):
        self.chain = chain

    def add(self, block): #Adding a block to the chain
        self.chain.append(block)

    def remove(self, block): #Removing a block from the chain
        self.chain.remove(block)

    def mine(self, block): #Mining block function
        try:
            block.previous_hash = self.chain[-1].hash()
        except IndexError:
            pass

        while True:
            if block.hash()[:self.difficulty] == "0" * self.difficulty:
                self.add(block); break

            else:
                block.nonce += 1

    def isValid(self): #Fucntion for validating blockchain
        for i in range(1, len(self.chain)):
            _previous = self.chain[i].previous_hash
            _current = self.chain[i-1].hash()
            if _previous != _current or _current[:self.difficulty] != "0" * self.difficulty: 
                return False

        return True



def main():
    blockchain = Blockchain()
    database = ["hello world", "what's up", "hello", "bye"]

    num = 0

    for data in database:
        num += 1
        blockchain.mine(Block(data, num))

    for block in blockchain.chain:
        print(block)


    blockchain.chain[2].data = "NEW DATA"
    blockchain.mine(blockchain.chain[2])
    print(blockchain.isValid())




if __name__ == '__main__':
    main()


