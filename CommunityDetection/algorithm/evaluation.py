def ber(predict,truth,nodes):
    predict=set(predict)
    truth=set(truth)
    nodes=set(nodes)
    fp=len(predict-truth)/float(len(predict))

    predict_out=nodes-predict
    truth_out=nodes-predict
    fn=len(predict_out-truth_out)/float(len(predict_out))

    balance_error_rate=0.5*(fp+fn)
    return balance_error_rate

def mapping(predict,truths,nodes):
    similarity=[]
    for truth in truths:
        er=ber(predict,truth,nodes)
        similarity.append((truth,er))
    similarity=sorted(similarity,key=lambda item:item[1])
    most_accuret=1-similarity[0][1]
    return  most_accuret

def evaluation(predictions,truths,nodes):
    accuracy=[]
    for predict in predictions:
        map_predict=mapping(predict,truths,nodes)
        accuracy.append(map_predict)
    fenmu=len(predictions)
    overall=sum(accuracy)/fenmu
    return overall

