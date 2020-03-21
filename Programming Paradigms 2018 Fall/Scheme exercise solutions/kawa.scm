#!/home/levan/Desktop/kawa-3.0/bin/kawa

;Factorial
(define (fact n)
    (if (zero? n) 1
        (* n (fact (- n 1)))
    )
)

;Fibonacci
(define (fib n)
    (if (<= n 1) n
        (+ (fib(- n 1)) (fib(- n 2)))
    )
)

;Reverse a list
(define (reverseList ls)
    (if (null? ls) '()
        (cons (getLast ls) (reverseList(withoutLast ls)))
    )
)

;List without its last element
(define (withoutLast seq)
    (if (or (null? seq) (= 1 (length seq)))'()
        (cons (car seq) (withoutLast(cdr seq)))    
    )
)

;Last element of a list
(define (getLast seq)
    (cond ((null? seq) '())
        ((= 1 (length seq)) (car seq))
        (else (getLast (cdr seq)))
    )
)

;Nth element of a list
(define (getNth seq n)
    (if (= n 0)
        (car seq)
        (getNth (cdr seq) (- n 1))
    )
)

;Length of a list
(define (myLength l) 
    (if (null? l) 
      0 
      (+ 1 (myLength (cdr list)))
    )
) 

;Checks for palindrome
(define (isPalindrome? seq)
    (if (null? seq) #t
        (if (not(= (car seq) (getLast seq))) #f
            (isPalindrome?(withoutLast(cdr seq)))
        )
    )
)

;Flatten a list
(define (flatten seq)
    (cond ((null? seq) '())
        ((list? (car seq)) (append (flatten(car seq)) (flatten (cdr seq))))
        (else (cons (car seq) (flatten(cdr seq))))
    )
)

;Wrapper function for below
(define (elimWrapper seq last)
    (if (null? seq) '()
        (if (= last (car seq))
            (elimWrapper(cdr seq)(car seq))
            (cons (car seq) (elimWrapper (cdr seq) (car seq)))
        )
    )
)

;Eliminates consecutive duplicates
(define (elimConsDup seq)
    (if (null? seq) '()
        (cons (car seq) (elimWrapper (cdr seq) (car seq)))
    )
)

;Wrapper function for below
(define (odd_wrapper num sec)
    (if (= (+ num 1) sec) 0
        (if (and(zero? (remainder num sec)) (= 1 (remainder sec 2))) (+ sec (odd_wrapper num (+ 1 sec)))
            (odd_wrapper num (+ 1 sec))
        )
    )
)

;Calculates sum of odd divisors for n
(define (odd_divisors n)
    (odd_wrapper n 1)
)

;Wrapper for encode function
(define (encode_wrapper seq num last)
    (cond ((null? seq) '())
        ((equal? 1 (length seq)) (list(cons (+ 1 num) (list last))))
        ((equal? last (car seq)) (encode_wrapper (cdr seq) (+ 1 num) (car seq)))
        (else (cons (cons num (list last))(encode_wrapper(cdr seq) 1 (car seq))))
    )
)

;(encode '(a a a a b c c a a d e e e e)) => ((4 a) (1 b) (2 c) (2 a) (1 d) (4 e)) 
(define (encode seq)
    (if(null? seq) '()
        (encode_wrapper seq 0 (car seq))
    )
)

;Wrapper for below
(define (dec_wrapper seq n)
    (if (= n (car seq)) '()
        (cons (cadr seq) (dec_wrapper seq (+ 1 n)))
    )
)

;(4 "a") -> (a a a a)
(define (deconstruct seq)
    (if (not(list? seq)) (list seq)
        (dec_wrapper seq 0)
    )
)

;Reverse of encode
(define (decode seq)
    (apply append(map (lambda (ls) (deconstruct ls)) seq))
)

;Wrapper for below function
(define (n_wrapper elem n)
    (if (zero? n) '()
        (cons elem (n_wrapper elem (- n 1)))
    )
)

;((1 2 3 4) 3) -> (1 1 1 2 2 2 3 3 3 4 4 4)
(define (n_elems seq n)
    (apply append (map (lambda (elem) (n_wrapper elem n)) seq))
)

;Remove every nth element from a list, rem is initialized as 1 when function is invoked
;(removeEveryNth '(1 2 3 4 5 6 7 8 9) 3) -> (3 6 9)
(define (removeEveryNth seq n rem)
    (cond ((null? seq) '())
        ((not(zero? (remainder rem n))) (removeEveryNth (cdr seq) n (+ 1 rem)))
        (else (cons (car seq) (removeEveryNth (cdr seq) n (+ 1 rem))))
    )
)

;Helper
(define (firstN seq n)
    (if (not (zero? n)) (cons (car seq) (firstN (cdr seq) (- n 1)))
        '()
    )
)

;Helper
(define (lastN  seq n)
    (if (null? seq) '()
        (if (= n 1) (cdr seq)
            (lastN (cdr seq) (- n 1))
        )
    )
)

;Split a list in 2 lists, size of first is given
;(split '(1 2 3 4 5 6 7 8) 3) -> '((1 2 3) (4 5 6 7 8))
(define (split seq n)
    (cons (firstN seq n) (list(lastN seq n)))
)

;Slice a list from start to end (including), counting indexes starts from 1, index is a wrapper, initially 1
;(slice '(1 2 3 4 5 6 7 8 9) 3 7 1) -> '(3 4 5 6 7)
(define (slice seq start end index)
    (cond ((< index start) (slice (cdr seq) start end (+ 1 index)))
        ((> index end) '())
        (else (cons (car seq) (slice (cdr seq) start end (+ 1 index))))
    )
)

;Removes nth element
(define (removeNth seq n)
    (cond ((null? seq) '())
        ((= n 1) (cdr seq))
        (else (cons (car seq) (removeNth (cdr seq) (- n 1))))
    )
)

;Generate subsets of list
;(subs '(1 2 3)) -> '(() (3) (2) (2 3) (1 2 3) (1 2) (1 3))
(define (subs seq)
    (if (null? seq) '(())
        (append (map (lambda (ls) (cons (car seq) ls)) (subs(cdr seq))) (subs(cdr seq)))
    )
)

;Removes first occurance of n
(define (removeFirstOcc seq n)
    (if (= n (car seq)) (cdr seq)
        (cons (car seq) (removeFirstOcc (cdr seq) n))
    )
)

;Permutations of a list
;(perms '(1 2 3)) -> '((1 2 3) (1 3 2) (2 1 3) (2 3 1) (3 1 2) (3 2 1)) 
(define (perms seq)
    (if (null? seq) '(())
        (apply append(map 
            (lambda (elem) 
                (map (lambda (ls) (cons elem ls)) (perms(removeFirstOcc seq elem)))
            ) 
        seq))
    )
)

;Removes every occurance of n from seq
(define (removeEvery seq n)
    (cond ((null? seq) '())
        ((equal? n (car seq)) (removeEvery(cdr seq) n))
        (else (cons(car seq) (removeEvery(cdr seq)n)))
    )
)

;Removes all duplicates from a list
(define (removeDup seq)
    (if (null? seq) '()
        (cons (car seq) (removeDup(removeEvery (cdr seq) (car seq))))
    )
)

;Count of an element in a list
(define (count seq n)
    (cond ((null? seq) 0)
        ((equal? n (car seq)) (+ 1 (count (cdr seq) n)))
        (else (count (cdr seq) n))
    )
)

;Generates histogram of a list
;(1 2 3 4 1 2 3) -> ((1 2) (2 2) (3 2) (4 1))
(define (histogram seq)
    (removeDup(map (lambda (elem) (cons elem (list(count seq elem)))) seq))
)

;Union of many sets
;(union '((1 2) (2 4) (4 8))) -> (1 2 4 8)
(define (union seq)
    (removeDup(apply append seq))
)

;Returns elem if is in a list, else empty list
(define (contains elem seq)
    (cond ((null? seq) '())
        ((equal? (car seq) elem) elem)
        (else (contains elem (cdr seq)))
    )
)

;Same but boolean
(define (contains? elem seq)
    (cond ((null? seq) #f)
        ((equal? (car seq) elem) #t)
        (else (contains? elem (cdr seq)))
    )
)

;Intersection of two sets
;(intersect '(a b c) '(a b d e)) -> (a b)
(define (intersect first second)
    (removeEvery(apply append(map (lambda (elem) (list(contains elem second))) first)) '())
)

;Wrapper for isWay
(define (isWayWrapper graph cur dest visited)
    (cond ((= cur dest) #t)
        ((contains? cur visited) #f)
        (else (map (lambda (pair) 
            (if (= cur (car pair)) (isWayWrapper graph (car (cdr pair)) dest (cons cur visited))
                #f
            )    
        ) graph))
    )
)

;Is there a way between 2 vertices in a graph
;>(isway? ((1 2) (1 3) (2 4)) 1 4) -> #t
(define (isWay? graph start dest)
    (contains? #t (flatten (isWayWrapper graph start dest '())))
)

;Checks if all elemens in sequence (of lists) are null
(define (allAreNull? seq)
    (not(contains? #f (map (lambda (ls) (null? ls)) seq)))
)

;Same as getLast, but returns 0 instead of '()
(define (getLastNum seq)
    (cond ((null? seq) 0)
        ((= 1 (length seq)) (car seq))
        (else (getLastNum (cdr seq)))
    )
)

;Sum of last elements of lists in a sequence
(define (sumOfCdrs seq carry)
    (+ carry (apply + (map (lambda (num) (getLastNum num)) seq)))
)

;Removes last elements from a sequence of lists
(define (removeLastElems seq)
    (map (lambda (num) 
        (if (null? num) '()
            (withoutLast num)
        )    
    ) seq)
)

;Returns 1 or '()
(define (lastCarry carry)
    (if (= carry 1) (list 1)
        '()
    )
)

;Addition of long numbers, needed 5 helper functions, written above, carry is 0 when function is invoked
;(addition '((4) (3 3) (1 2 3) (4 3 2 1)) 0) -> (4 4 8 1)
(define (addition seq carry)
    (if (allAreNull? seq) (lastCarry carry)
        (if (> (sumOfCdrs seq carry) 9) 
            (append (addition (removeLastElems seq) 1) (list (remainder (sumOfCdrs seq carry) 10)))
            (append (addition (removeLastElems seq) 0) (list (remainder (sumOfCdrs seq carry) 10)))
        )
    )
)

;(marry'("a" "b" "c" "d" "e" "f")) -> ((a b)(c d)(e f))
(define (marry seq)
    (cond
        ((null? seq) '()) 
        ((= 1 (length seq)) (list(list(car seq))))
            (else 
                (cons (cons (car seq) (list (cadr seq))) (marry (cddr seq)))
            )  
    )
)

;(mdp length '(a b c d)) -> (4 3 2 1)
(define (mdp func seq)
    (if (null? seq) '()
        (cons (func seq) (mdp func (cdr seq)))
    )
)

;(longest_common_prefix '(1 2 3 4 5) '(1 2 4 3 5)) -> (1 2)
(define (longest_common_prefix a b)
    (cond ((or(null? a)(null? b)) '())
        ((equal? (car a)(car b)) (cons (car a) (longest_common_prefix (cdr a)(cdr b))))
        (else '())
    )
)

;makes a list from 2 to end(not including)
(define (makeList start end)
    (if (= start end) '()
        (cons start (makeList (+ 1 start) end))
    )
)

;Generates a list of booleans about numbers divisors
(define (primeList n)
    (map (lambda (num) 
        (if (zero? (remainder n num)) #f
            #t
        )
    ) (makeList 2 n))
)

;Checks whether number is prime
(define (isPrime n)
    (cond ((<= n 3) #t)
        ((contains? #f (primeList n))#f)
        (else #t)
    )
)

;Lists prime numbers from cur to end (including both)
(define (listPrimes cur end)
    (cond ((> cur end) '())
        ((isPrime cur) (cons cur (listPrimes (+ 1 cur) end)))
        (else (listPrimes (+ 1 cur) end))
    )
)

;Chooses subsets of length k
(define (kSubsets seq k)
    (apply append (map (lambda (subset) 
        (if (= k (length subset)) (list subset)
            '()
        )
    ) (subs seq)))
)

;Sum of pairs for this element
(define (getValue seq first sum)
    (cond ((null? seq) 0)
        ((>= (+ sum (car seq)) (* first (car seq)))  (+ 1 (getValue (cdr seq) first (+ sum (car seq)))))
        (else (getValue (cdr seq) first (+ sum (car seq))))
    )
)

;Wrapper function
(define (pairsW seq sum)
    (if (>= 1 (length seq)) 0
        (+ (getValue (cdr seq) (car seq) sum) (pairsW (cdr seq) (+ sum (cadr seq))))
    )
)

;Pairs from 3rd midterm of 2017
(define (pairs seq)
    (pairsW seq (car seq))
)

;Wrapper function for swap, uses slices and getNth methods 
(define (swapW seq first second)
    (append (slice seq 1 (- first 1) 1) (list (getNth seq (- second 1))) (slice seq (+ 1 first) (- second 1) 1)
        (list (getNth seq (- first 1))) (slice seq (+ 1 second) (length seq) 1)
    )
)

;Swaps two elements in a list
(define (swap seq first second)
    (if (> first second) (swapW seq second first)
        (swapW seq first second)     
    )
)

;Calculates all the sums
(define (allSumWrapper n ls curLs)
    (cond ((< n 0) '())
        ((zero? n) (list curLs))
        (else (apply append(map (lambda (cur) 
            (allSumWrapper (- n cur) ls (cons cur curLs))) 
        ls)))
    )
)

;Wrapper function
(define (allSumFromList n ls)
    (allSumWrapper n ls '())
)

;(makeListTillN 7) -> (1 2 3 4 5 6)
(define (makeListTillN n)
    (if (zero? n) '()
        (cons n (makeListTillN (- n 1)))
    )
)

;(allSum 3) -> ((1 2) (2 1) (1 1 1))
;(allSum 4) -> ((1 1 1 1) (1 1 2) (1 2 1) (1 3) (2 2) (2 1 1) (3 1))
(define (allSum n)
    (allSumFromList n (makeListTillN (- n 1)))
)

; Example of a binary tree - root is 56 ( ( (() 22 ()) 34 () ) 56 ( ( () 59 (() 74 ()) ) 88 () ) )
(define (left tree)(car tree)) ;left child
(define (root tree)(cadr tree)) ;right child
(define (right tree)(caddr tree)) ;root

;Min element of a tree    
(define (minTree tree)
    (if (null? (left tree)) (root tree)
        (minTree (left tree))
    )
)

;Max element of a tree
(define (maxTree tree)
    (if (null? (right tree)) (root tree)
        (maxTree (right tree))
    )
)

;Checks whether an element is in a binary tree
(define (containsTree? tree elem)
    (cond 
        ((null? tree) #f)
        ((equal? elem (root tree)) #t)
        ((> elem (root tree)) (containsTree? (right tree) elem))
        (else (containsTree? (left tree) elem))
    )
)

;Inserts an element in a binary tree
(define (insertTree tree elem)
    (cond
        ((null? tree) (list '() elem '()))
        ((> elem (root tree)) (apply append (list (withoutLast tree) (list (insertTree (right tree) elem)))))
        (else (cons (insertTree (left tree) elem) (cdr tree)))
    )
)

(define getMinWrapper seq curMin)
    (cond ((null? seq) curMin)
        (( < (cadar seq) (cadr curMin)) (getMinWrapper (cdr seq) (car seq)))
        (else (getMinWrapper (cdr seq) curMin))
    )
)

(define (getMin seq)
    (getMinWrapper seq (car seq))
)

(define salesWrapper n graph cur weight visited)
    (if (= n (length visited)) (list (list (reverse visited) weight))
        (apply append (map (lambda (vertex)
            (if (and (= cur (car vertex)) (not (contains? (cadr vertex) visited)))
                (salesWrapper n graph (cadr vertex) (+ weight (caddr vertex)) (cons (cadr vertex) visited))
                '()
             )
         ) graph))
    )
)


(define (salesman n graph)
    (car (getMin (salesWrapper n graph 1 0 '(1))))  
)

(   
    display(isWay? '((1 2) (1 3) (2 4)) 1 4)
) (newline)
