//
// Copyright SpiRITlab - The Panther project.
// https://github.com/SpiRITlab/Panther
//

#include "XCode.h"
#include "util.h"
#include "Cache.h"
#include <string>
#include <vector>

/*
 * XCodeTableBase
 */

template<class T>
Panther::XCodeTableBase<T>::XCodeTableBase(int cols, int rows) : _columns(cols), _rows(rows) {
}

template<class T>
T
Panther::XCodeTableBase<T>::get(int colID, int rowID) {
    return xcodeTable[colID][rowID];
}

template<class T>
void
Panther::XCodeTableBase<T>::assign(int colID, int rowID, T val) {
    xcodeTable[colID][rowID] = val;
}

template<class T>
void
Panther::XCodeTableBase<T>::print() {
    for (int c = 0; c < _columns; ++c) {
        for (int r = 0; r < _rows; ++r) {
            std::cout << xcodeTable[r][c] << "  ";
        }
        std::cout << std::endl;
    }
}

template<class T>
void
Panther::XCodeTableBase<T>::swtichLastTwoRows() {
    T tmp;
    for (int c = 0; c < _columns; ++c) {
        tmp = xcodeTable[c][_rows - 2];
        xcodeTable[c][_rows - 2] = xcodeTable[c][_rows - 1];
        xcodeTable[c][_rows - 1] = tmp;
    }
}

/***/
template<class T>
Panther::XCodeLabelTable<T>::XCodeLabelTable(int cols, int rows) : XCodeTableBase<T>(cols, rows) {
    for (int c = 0; c < cols; ++c) {
        std::vector<T> column;
        for (int r = 0; r < rows; ++r) {
            column.push_back(std::to_string(r) + ',' + std::to_string(c));
        }
        this->xcodeTable.push_back(column);
    }
}

/*
 *
 */
template<class T>
Panther::XCodeDataTable<T>::XCodeDataTable(int cols, int rows) : XCodeTableBase<T>(cols, rows) {
    for (int c = 0; c < cols; ++c) {
        std::vector<T> column(rows);
        this->xcodeTable.push_back(column);
    }
}


/*
 * XCode
 */

template<class T>
Panther::XCode<T>::XCode(int n) : n(n) {
    this->part_1_labelTable = new Panther::XCodeLabelTable<std::string>(n, n);
    this->part_2_labelTable = new Panther::XCodeLabelTable<std::string>(n, n);
}

template<class T>
Panther::XCodeLabelTable<std::string> *
Panther::XCode<T>::getPart1LabelTable() {
    return part_1_labelTable;
}

template<class T>
Panther::XCodeLabelTable<std::string> *
Panther::XCode<T>::getPart2LabelTable() {
    return part_2_labelTable;
}

template<class T>
void
Panther::XCode<T>::printPart1LabelTable() {
    part_1_labelTable->print();
}

template<class T>
void
Panther::XCode<T>::printPart2LabelTable() {
    part_2_labelTable->print();
}

template<class T>
void
Panther::XCode<T>::encode(std::vector<Panther::Shard<T>> &shards, Panther::Dataset<T> &dataset) {
    int cols = n, rows = n;

    Panther::XCodeDataTable<bool> xCodeDataTable(n, n);
    for (int c = 0; c < cols; ++c) {
        for (int r = 0; r < rows - 2; ++r) {
            xCodeDataTable.assign(c, r, dataset.get(r, c));
        }
    }
    util::debug("Filling xCodeDataTable...");
    xCodeDataTable.print();

    // calculate parity bits
    bool part1Tmp, part2Tmp;
    int part1RowID = rows - 2;
    int part2RowID = rows - 1;
    for (int c = 0; c < cols; ++c) {
//        std::cout << "part 1: " << mod(part1RowID-1,rows) << " " << mod(c+1,cols)  << std::endl;
        part1Tmp = xCodeDataTable.get(mod(c + 1, cols), mod(part1RowID - 1, rows));
//        std::cout << "part 2: " << mod(part2RowID-2,rows) << " " << mod(c-1,cols) << std::endl;
        part2Tmp = xCodeDataTable.get(mod(c - 1, cols), mod(part2RowID - 2, rows));

        for (int r = 2; r < rows - 1; ++r) {
//            std::cout << "part 1: " << (part1RowID-r)%rows << " " << (c+r)%cols  << std::endl;
            part1Tmp ^= xCodeDataTable.get(mod(c + r, cols), mod(part1RowID - r, rows));
//            std::cout << "part 2: " << mod(part2RowID-r-1,rows) << " " << mod(c-r,cols)  << std::endl;
            part2Tmp ^= xCodeDataTable.get(mod(c - r, cols), mod(part2RowID - r - 1, rows));
        }
        xCodeDataTable.assign(c, part1RowID, part1Tmp);
        xCodeDataTable.assign(c, part2RowID, part2Tmp);
    }

    util::debug("Calculated parity bits.");
    xCodeDataTable.print();

    int rotationCnt = 0;
    Panther::XCodeLabelTable<std::string> originalLabelMap(*part_1_labelTable);
    util::debug("Part 1: ");
    for (int c = 0; c < cols; ++c) {
        Panther::Shard<T> shard;
        rotationCnt = (cols - 1) - c;
        for (int r = 0; r < rows; ++r) {
            shard.addToPart1(xCodeDataTable.get(c, (r + rotationCnt) % rows));
            part_1_labelTable->assign(c, r, originalLabelMap.get(c, (r + rotationCnt) % rows));
        }
        shards.push_back(shard);
        std::cout << "shard" << c << ": ";
        shard.printPart1();
    }

    util::debug("Part 2: ");
    xCodeDataTable.swtichLastTwoRows();
    originalLabelMap.swtichLastTwoRows();
    for (int c = 0; c < cols; ++c) {
        rotationCnt = c;
        for (int r = 0; r < rows; ++r) {
            shards[c].addToPart2(xCodeDataTable.get(c, (r + rotationCnt) % rows));
            part_2_labelTable->assign(c, r, originalLabelMap.get(c, (r + rotationCnt) % rows));
        }
        std::cout << "shard" << c << ": ";
        shards[c].printPart2();
    }
}


template<class T>
void
Panther::XCode<T>::recoverFromOneColumnErasure(Panther::Encryptor &encryptor, std::vector<Panther::Shard<T>> &shards,
                                               int colID) {
    int rows = n, cols = n;
    helib::Ctxt recoveredShard_part1(*(encryptor.getPublicKey()));
    helib::Ctxt recoveredShard_part2(*(encryptor.getPublicKey()));
    int colIdx;
    std::string label;
    Panther::Cache cache;

    // part 1
    util::debug("Recovering part 1 with one column erasure...\n\n");
    for (int r = 0; r < rows; ++r) {
        std::cout << "Before loop: " << r << "," << colID << std::endl;;
        if (r != colID) { // skip one computation for the parity bit
            std::cout << "Going in loop for: " << r << "," << colID << std::endl;;
            helib::Ctxt ctxt(*(encryptor.getPublicKey()));
            helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
            for (int c = 1; c < cols; ++c) {
                colIdx = mod(colID + c, cols);
                std::cout << "ColID Before" << r << "," << colIdx << std::endl;
                if (colIdx != r) {
                    std::cout << "ColID After" << r << "," << colIdx << std::endl;
                    std::cout << "row: " << r << " colIdx: " << colIdx << std::endl;
//                   encryptor.decryptAndPrint("ctxt_before", ctxt);
//                   encryptor.decryptAndPrint("shardctxt", *(shards[colIdx].getPart1Ctxt()));
                    ctxt.addCtxt(*(shards[colIdx].getPart1Ctxt()));
//                   encryptor.decryptAndPrint("ctxt_after", ctxt);
                }
            }
            bitmask[r] = 1;
            std::cout << "bitmask: " << bitmask << std::endl;
            ctxt.multByConstant(bitmask);
//            encryptor.decryptAndPrint("ctxt_after_bitmask", ctxt);
            recoveredShard_part1.addCtxt(ctxt);
//            encryptor.decryptAndPrint("recoveredShard_part1_adding", recoveredShard_part1);
            std::cout << "In Rotated Matrix: " << r << "," << colID << std::endl;
            label = this->part_1_labelTable->get(colID, r);
            std::cout << "Actual Matrix (label): " << label << std::endl;
            cache.add(label, new Panther::CacheEntry(r, colID, new helib::Ctxt(ctxt)));
        }
    }
    cache.print();
    // part 2
    util::debug("Recovering part 2...");
    for (int r = 0; r < rows; ++r) {
        if (r != rows - colID - 1) { // skip one computation for the parity bit
            label = this->part_2_labelTable->get(colID, r);
//            std::cout << "label: " << label << std::endl;
            if (!cache.contain(label)) {
                util::debug("computed value doesn't exist, compute it now.");
                helib::Ctxt ctxt(*(encryptor.getPublicKey()));
                helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
                for (int c = 1; c < cols; ++c) {
                    colIdx = mod(colID + c, cols);
                    if (colIdx != rows - r - 1) {
                        std::cout << "row: " << r << " colIdx: " << colIdx << std::endl;
                        ctxt.addCtxt(*(shards[colIdx].getPart2Ctxt()));
                    }
                }
                bitmask[r] = 1;
                ctxt.multByConstant(bitmask);
                recoveredShard_part2.addCtxt(ctxt);

                label = this->part_2_labelTable->get(colID, r);
                cache.add(label, new Panther::CacheEntry(r, colID, new helib::Ctxt(ctxt)));
            } else { // if found, this means it has been computed in part 1
                util::debug("reusing computed value.");
//               encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
                std::cout << "rotation " << r - cache.get(label)->getRowID() << std::endl;
//                encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), r - cache.get(label)->getRowID());
//               encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
                recoveredShard_part2.addCtxt(*(cache.get(label)->getCtxt()));
            }
        }
    }

    cache.print();

    util::debug("populating missing values...");
    label = this->part_1_labelTable->get(colID, colID);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << colID - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), colID - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part1.addCtxt(*(cache.get(label)->getCtxt()));

    label = this->part_2_labelTable->get(colID, rows - colID - 1);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << rows - colID - 1 - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()),
                                          rows - colID - 1 - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part2.addCtxt(*(cache.get(label)->getCtxt()));

    encryptor.decryptAndPrint("RecoveredShardPart1", recoveredShard_part1);
    encryptor.decryptAndPrint("RecoveredShardPart2", recoveredShard_part2);
}

template<class T>
void
Panther::XCode<T>::recoverFromTwoColumnErasure(Panther::Encryptor &encryptor, std::vector<Panther::Shard<T>> &shards,
                                               int colID1, int colID2) {
    int rows = n, cols = n;
    helib::Ctxt recoveredShard_part1(*(encryptor.getPublicKey()));
    helib::Ctxt recoveredShard_part2(*(encryptor.getPublicKey()));
    std::string label;
    std::string label1;
    Panther::Cache cache;
    std::vector<std::string> failedElements;
    int diagonalCount = 0;
    int failedPerRow = 0;
    int inCache = 0;
    int colIdx1 = 0;
    int colIdx2 = 0;
    bool flag = false;

    for (int i = 0; i < n; i++) {
        failedElements.push_back((std::to_string(i) + std::to_string(colID1)));
        failedElements.push_back((std::to_string(i) + std::to_string(colID2)));
    }
    for (int i = 0; i < failedElements.size(); i++) {
        std::cout << "Updated Failed Nodes: " << failedElements[i] << std::endl;
    }

    // part 1
    util::debug("\n========== Recovering part 1 with TWO Column Erasure =========\n");
//    while (!failedElements.empty()) {
    int i = 0;
    while (i < 4) {
        std::cout << "----------------------  Entering next Iteration: " <<i<< std::endl;
        for (int r = 0; r < rows; ++r) {
//        if (r == colID1 || r == colID2) { // go only with the diognal parity ones
            diagonalCount = 0;
            failedPerRow = 0;
            inCache = 0;
            helib::Ctxt ctxt(*(encryptor.getPublicKey()));
            helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
            std::string tempCoordinates = "0";
            for (int c = 1; c < cols; ++c) {
                tempCoordinates = (std::to_string(r) + std::to_string(c));
                if (std::find(failedElements.begin(), failedElements.end(), tempCoordinates) != failedElements.end()) {
                    colIdx1 = mod(colID1 + c, cols);
                    colIdx2 = mod(colID2 + c, cols);

                    std::cout << "colIdx1 is:" << r << "," << colIdx1 << std::endl;
                    std::cout << "colIdx2 is:" << r << "," << colIdx2 << std::endl;

//                    label = this->part_1_labelTable->get(colID1, r);
//                    if (cache.contain(label)) {
//                        std::cout << "HURRAH!! " << label << " IS FOUND IN CACHE for coldID1: " << std::endl;
//                        inCache++;
//                        ctxt.addCtxt(*(shards[colID1].getPart1Ctxt()));
//                        // if found, this means it has been computed in part 1
////                        util::debug("reusing computed value.");
//                        encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
//                        label = this->part_1_labelTable->get(colID1, r);
//                        std::cout << "rotation " << r - cache.get(label)->getRowID() << std::endl;
//                        encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()),
//                                                              r - cache.get(label)->getRowID());
//                        encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
//                        recoveredShard_part1.addCtxt(*(cache.get(label)->getCtxt()));
//                        std::cout << "exiting cache loop 1 left matrix" << std::endl;
//                    }
//
//                    label = this->part_1_labelTable->get(colID2, r);
//                    if (cache.contain(label)) {
//                        std::cout << "HURRAH!! " << label << " IS FOUND IN CACHE for coldID2: " << std::endl;
//                        inCache++;
//                        ctxt.addCtxt(*(shards[colIdx2].getPart1Ctxt()));
//                        // if found, this means it has been computed in part 1
//                        util::debug("reusing computed value.");
////                        encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
//                        label = this->part_1_labelTable->get(colID2, r);
//                        std::cout << "rotation " << r - cache.get(label)->getRowID() << std::endl;
//                        encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()),
//                                                              r - cache.get(label)->getRowID());
////                        encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
//                        recoveredShard_part1.addCtxt(*(cache.get(label)->getCtxt()));
//                        std::cout << "exiting cache loop 2 left matrix" << std::endl;
//                    }
                    if (r == c) {
                        std::cout << "Diagonal broken element found: " << tempCoordinates << std::endl;
                        diagonalCount++;}
//                    } else if (std::find(failedElements.begin(), failedElements.end(), tempCoordinates) ==
//                               failedElements.end()) {
                    else{
                        std::cout << "broken element found: " << tempCoordinates << std::endl;
                        failedPerRow++;
                    }
                    std::cout << "FailedPerRow Count: " << failedPerRow << std::endl;
                    std::cout << "Diagonal Count: " << diagonalCount << std::endl;
                    std::cout << "Cache Count: " << inCache << std::endl << std::endl;
                }
            }
            if (failedPerRow > 1 && diagonalCount == 0) {
                std::cout << "Skipping this row - More than one element is faulty and not diagonal" << std::endl;;
                continue;
            } else {
                flag = true;
                std::cout << "----FailedPerRow Count----: " << failedPerRow << std::endl;
                std::cout << "----Diagonal Count-------: " << diagonalCount << std::endl << std::endl;
                std::cout << "colIDx1:   " << colIdx1 << std::endl;
                std::cout << "ColID1 Before" << r << "," << colIdx1 << std::endl;
                std::cout << "ColID2 Before" << r << "," << colIdx2 << std::endl;
                if (colIdx1 != r && colIdx2 != r) {
                    std::cout << "ColID1 After: " << r << "," << colIdx1 << std::endl;
                    std::cout << "ColID2 After" << r << "," << colIdx2 << std::endl;
//                    encryptor.decryptAndPrint("ctxt_before", ctxt);
//                    encryptor.decryptAndPrint("shardctxt", *(shards[colIdx1].getPart1Ctxt()));
                    ctxt.addCtxt(*(shards[colIdx1].getPart1Ctxt()));
                    ctxt.addCtxt(*(shards[colIdx2].getPart1Ctxt()));

//                    encryptor.decryptAndPrint("ctxt_after", ctxt);
                }
            }
            if (flag == true) {
                std::cout << "########### APPLYING BIT MASK NOW ON : #############" << std::endl;
                std::cout << r << "," << colIdx1 << std::endl;
                bitmask[r] = 1;
//                std::cout << "bitmask: " << bitmask << std::endl;
                ctxt.multByConstant(bitmask);
//                encryptor.decryptAndPrint("ctxt_after_bitmask", ctxt);
                recoveredShard_part1.addCtxt(ctxt);
//                encryptor.decryptAndPrint("recoveredShard_part1_adding", recoveredShard_part1);
                std::cout << "In Rotated Matrix: " << r << "," << colID1 << std::endl;
                if (r != colID1) {
                    std::cout << "Actual Matrix (label) for colID1: " << label << std::endl;
                    label = this->part_1_labelTable->get(colID1, r);
                    std::cout << "Actual Matrix (label): " << label << std::endl;
                    cache.add(label, new Panther::CacheEntry(r, colID1, new helib::Ctxt(ctxt)));
                    std::string tempCoord = std::to_string(r) + std::to_string(colID1);
                    std::cout << "ALERT!!!! REMOVING: " << tempCoord << std::endl;
                    failedElements.erase(std::remove(failedElements.begin(), failedElements.end(), tempCoord),
                                         failedElements.end());

                    for (int i = 0; i < failedElements.size(); i++) {
                        std::cout << "Updated Failed Nodes: " << failedElements[i] << std::endl;
                    }
                }
                if (r != colID2) {
                    std::cout << "Actual Matrix (label) for colID2: " << label << std::endl;
                    label = this->part_1_labelTable->get(colID2, r);
                    cache.add(label, new Panther::CacheEntry(r, colID2, new helib::Ctxt(ctxt)));
                    std::string tempCoord = std::to_string(r) + std::to_string(colID2);
                    std::cout << "ALERT!!!! REMOVING: " << tempCoord << std::endl;
                    failedElements.erase(std::remove(failedElements.begin(), failedElements.end(), tempCoord),
                                         failedElements.end());

                    for (int i = 0; i < failedElements.size(); i++) {
                        std::cout << "Updated Failed Nodes: " << failedElements[i] << std::endl;
                    }
                }
                cache.print();
            }

            //} end if loop for parity was here

        }
        // part 2
        util::debug("### --- RECOVERING RIGHT MATRIX ##### -----");
        for (int r = 0; r < rows; ++r) {
//        if (r == rows - colID1 - 1 || (r == rows - colID2 - 1)) { // go only with the diagonal parity ones
            diagonalCount = 0;
            failedPerRow = 0;
            label = this->part_2_labelTable->get(colID1, r);
            label1 = this->part_2_labelTable->get(colID2, r);
//            std::cout << "label: " << label << std::endl;
            if (!cache.contain(label) || !cache.contain(label1)) {
                util::debug("computed value doesn't exist, compute it now.");
                helib::Ctxt ctxt(*(encryptor.getPublicKey()));
                helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
                std::string tempCoordinates = "0";
                for (int c = 1; c < cols; ++c) {
                    tempCoordinates = (std::to_string(r) + std::to_string(c));
                    if (std::find(failedElements.begin(), failedElements.end(), tempCoordinates) !=
                        failedElements.end()) {
                        colIdx1 = mod(colID1 + c, cols);
                        colIdx2 = mod(colID2 + c, cols);
//                        if (cache.contain((std::to_string(r) + std::to_string(c)))) {
//                            std::cout << "HURRAH!! " << tempCoordinates << " IS FOUND IN CACHE: " << std::endl;
//                            inCache++;
//                            // if found, this means it has been computed in part 1
//                            util::debug("reusing computed value.");
////                          encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
//                            std::cout << "NEEED CHNAGED. ADDED ONLY COL 2" << std::endl;
//                            label = this->part_1_labelTable->get(colID2, r);
//                            std::cout << "rotation " << r - cache.get(label)->getRowID() << std::endl;
//                            //                  encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), r - cache.get(label)->getRowID());
//                            //                  encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
//                            recoveredShard_part2.addCtxt(*(cache.get(label)->getCtxt()));
//
//                        }
                        if (rows - c - 1 == r) {
                            std::cout << "Diagonal broken element found: " << tempCoordinates << std::endl;
                            diagonalCount++;
                        } else {
                            std::cout << "broken element found: " << tempCoordinates << std::endl;
                            failedPerRow++;
                        }
                    }
                }
                if (failedPerRow > 1 && diagonalCount == 0) {
                    std::cout << "Skipping this row: " << r << std::endl;;
                    continue;
                } else {
                    flag = true;
                    std::cout << "\n\n----FailedPerRow Count----: " << failedPerRow << std::endl;
                    std::cout << "----Diagonal Count-------: " << diagonalCount << std::endl << std::endl;
                    std::cout << "ColID1 Before" << r << "," << colIdx1 << std::endl;
                    std::cout << "ColID2 Before" << r << "," << colIdx2 << std::endl;
                    if (colIdx1 != (rows - r - 1) || colIdx2 != (rows - r - 1)) {
                        std::cout << "ColID1 After: " << r << "," << colIdx1 << std::endl;
                        std::cout << "ColID2 After" << r << "," << colIdx2 << std::endl;
//                  encryptor.decryptAndPrint("ctxt_before", ctxt);
//                  encryptor.decryptAndPrint("shardctxt", *(shards[colIdx1].getPart1Ctxt()));
                        ctxt.addCtxt(*(shards[colIdx1].getPart1Ctxt()));
                        ctxt.addCtxt(*(shards[colIdx2].getPart1Ctxt()));
//                  encryptor.decryptAndPrint("ctxt_after", ctxt);
                        bitmask[r] = 1;
                        ctxt.multByConstant(bitmask);
                        recoveredShard_part2.addCtxt(ctxt);
//                        encryptor.decryptAndPrint("recoveredShard_part2_adding", recoveredShard_part2);
                        std::cout << "In Rotated Matrix: " << r << "," << colID1 << std::endl;
                        std::cout << "In Rotated Matrix: " << r << "," << colID2 << std::endl;
                        if (r != rows - 1 - colID1) {
                            label = this->part_2_labelTable->get(colID1, r);
                            std::cout << "Actual Matrix (label) for colID1: " << label << std::endl;
                            cache.add(label, new Panther::CacheEntry(r, colID1, new helib::Ctxt(ctxt)));
                            std::string tempCoord = std::to_string(r) + std::to_string(colID1);
                            std::cout << "ALERT!!!! REMOVING: " << tempCoord << std::endl;
                            failedElements.erase(std::remove(failedElements.begin(), failedElements.end(), tempCoord),
                                                 failedElements.end());

                            for (int i = 0; i < failedElements.size(); i++) {
                                std::cout << "Updated Failed Nodes: " << failedElements[i] << std::endl;
                            }

                        }
                        if (r != rows - 1 - colID2) {
                            label = this->part_2_labelTable->get(colID2, r);
                            std::cout << "Actual Matrix (label) for colID2: " << label << std::endl;
                            cache.add(label, new Panther::CacheEntry(r, colID2, new helib::Ctxt(ctxt)));
                            std::string tempCoord = std::to_string(r) + std::to_string(colID2);
                            std::cout << "ALERT!!!! REMOVING: " << tempCoord << std::endl;
                            failedElements.erase(std::remove(failedElements.begin(), failedElements.end(), tempCoord),
                                                 failedElements.end());

                            for (int i = 0; i < failedElements.size(); i++) {
                                std::cout << "Updated Failed Nodes: " << failedElements[i] << std::endl;
                            }
                        }
                    }
                    cache.print();
                }
            }
//            } parity end wala brace

            else { // if found, this means it has been computed in part 1
                util::debug("reusing computed value.");
//               encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
                std::cout << "rotation " << r - cache.get(label)->getRowID() << std::endl;
//                encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), r - cache.get(label)->getRowID());
//               encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
                recoveredShard_part2.addCtxt(*(cache.get(label)->getCtxt()));
            }
        }
        cache.print();
        std::cout << " +++++++++++++++++++++++++++ REACHED THE END ++++++++++++++++++++++++++++++" << std::endl
                  << std::endl;
    i++;
        std::cout << "----------------------  Entering next Iteration: " <<i<< std::endl;
    }

    util::debug("populating missing values...");
    label = this->part_1_labelTable->get(colID1, colID1);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << colID1 - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), colID1 - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part1.addCtxt(*(cache.get(label)->getCtxt()));

    label = this->part_2_labelTable->get(colID1, rows - colID1 - 1);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << rows - colID1 - 1 - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()),
                                          rows - colID1 - 1 - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part2.addCtxt(*(cache.get(label)->getCtxt()));

    encryptor.decryptAndPrint("RecoveredShardPart1", recoveredShard_part1);
    encryptor.decryptAndPrint("RecoveredShardPart2", recoveredShard_part2);
}

template
class Panther::XCode<bool>;