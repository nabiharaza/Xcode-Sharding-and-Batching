//
// Copyright SpiRITlab - The Panther project.
// https://github.com/SpiRITlab/Panther
//

#include "XCode.h"
#include "util.h"
#include "Cache.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

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
        if (r != colID) { // skip one computation for the parity bit
            helib::Ctxt ctxt(*(encryptor.getPublicKey()));
            helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));


            for (int c = 1; c < cols; ++c) {
                std::cout << "----- For  row and Coloumn:  " << r << "," << c << "  ------" << std::endl << std::endl;
                colIdx = mod(colID + c, cols);
                std::cout << "ColIdx Before Check if colIdx !=r : " << r << "," << colIdx << std::endl;
                if (colIdx != r) {
                    std::cout << "ColIdx After" << r << "," << colIdx << std::endl;
                    std::cout << "row: " << r << " colIdx: " << colIdx << std::endl;
                    encryptor.decryptAndPrint("ctxt_before", ctxt);
                    encryptor.decryptAndPrint("shardctxt", *(shards[colIdx].getPart1Ctxt()));
                    ctxt.addCtxt(*(shards[colIdx].getPart1Ctxt()));
                    encryptor.decryptAndPrint("coldx ctxt_after", ctxt);
                }
            }
            bitmask[r] = 1;
            std::cout << "bitmask: " << bitmask << std::endl;
            encryptor.decryptAndPrint("part 1 ctxt_before_bitmask", ctxt);
            ctxt.multByConstant(bitmask);
            encryptor.decryptAndPrint("part 1 ctxt_after_bitmask", ctxt);
            recoveredShard_part1.addCtxt(ctxt);
            encryptor.decryptAndPrint("\nrecoveredShard_part1_adding", recoveredShard_part1);
            std::cout << "" << std::endl;
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
            std::cout << "label: " << label << std::endl;
            if (!cache.contain(label)) {
                util::debug("computed value doesn't exist, compute it now.");
                helib::Ctxt ctxt(*(encryptor.getPublicKey()));
                helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
                for (int c = 1; c < cols; ++c) {
                    colIdx = mod(colID + c, cols);
                    if (colIdx != rows - r - 1) {
                        std::cout << "row: " << r << " colIdx: " << colIdx << std::endl;
                        ctxt.addCtxt(*(shards[colIdx].getPart2Ctxt()));
                        encryptor.decryptAndPrint("After adding coldIDx ctxt", ctxt);

                    }
                }
                bitmask[r] = 1;
                encryptor.decryptAndPrint("part 2 ctxt_before_bitmask", ctxt);
                ctxt.multByConstant(bitmask);
                recoveredShard_part2.addCtxt(ctxt);
                encryptor.decryptAndPrint("part 2 ctxt_before_bitmask", ctxt);
                encryptor.decryptAndPrint("\nrecoveredShard_part2_adding", recoveredShard_part1);
                std::cout << "" << std::endl;

                label = this->part_2_labelTable->get(colID, r);
                cache.add(label, new Panther::CacheEntry(r, colID, new helib::Ctxt(ctxt)));
            } else { // if found, this means it has been computed in part 1
                util::debug("reusing computed value.");
                encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
                std::cout << "rotation " << r - cache.get(label)->getRowID() << std::endl;
                encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), r - cache.get(label)->getRowID());
                encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
                recoveredShard_part2.addCtxt(*(cache.get(label)->getCtxt()));
                encryptor.decryptAndPrint("\n\n\n-----!!!! recoveredShard_part2_adding", recoveredShard_part2);
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

    /*
     *
     * Two Disk Recovery
     */

    int rows = n, cols = n;
    helib::Ctxt recoveredShard_part1_disk1(*(encryptor.getPublicKey()));
    helib::Ctxt recoveredShard_part1_disk2(*(encryptor.getPublicKey()));

    helib::Ctxt recoveredShard_part2_disk1(*(encryptor.getPublicKey()));
    helib::Ctxt recoveredShard_part2_disk2(*(encryptor.getPublicKey()));
    std::string label;
    std::string label1;
    Panther::Cache cache;
    std::vector<std::string> failedElements;
    int colIdx1 = 0;
    int colIdx2 = 0;
    std::string brokenElementD1 = "";
    std::string brokenElementD2 = "";
    bool workingRowFlag = false;
    int rowOperationCount = 0;
    int iter = 0;
    std::vector<std::string> nodesRecoveredIter;
    std::vector<std::string> part1RecoveredRowCheck;
    std::vector<std::string> part2RecoveredRowCheck;

    std::cout << "--------------- Two Coloumn Recovery ------------------" << std::endl;

    while (iter < 3) {
        std::cout << "\n\n################### Entering next Iteration: " << iter << " ##################" << std::endl;
        nodesRecoveredIter.clear();
        std::cout << "At Start NodesIterSize: " << nodesRecoveredIter.size() << std::endl;
        //part 1
        for (int r = 0; r < rows; ++r) {
            helib::Ctxt ctxt(*(encryptor.getPublicKey()));
            helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
            workingRowFlag = false;
            rowOperationCount = 0;
            label = this->part_1_labelTable->get(colID1, r);
            label1 = this->part_1_labelTable->get(colID2, r);
            bool ans = false;
            bool ans1 = false;
            ans = (std::find(nodesRecoveredIter.begin(), nodesRecoveredIter.end(), label) != nodesRecoveredIter.end());
            ans1 = (std::find(nodesRecoveredIter.begin(), nodesRecoveredIter.end(), label1) !=
                    nodesRecoveredIter.end());
            std::cout << "ANS BOOL ANS: " << ans << std::endl;


            if (std::find(part1RecoveredRowCheck.begin(), part1RecoveredRowCheck.end(), std::to_string(r)) !=
                part1RecoveredRowCheck.end() == 0) {
                std::cout << "\n\nNote: This row/element has never recovered before\n\n";
                if (cache.contain(label) && (ans == 0)) { //check if disk1 element is in Cache already
                    std::cout << "Reusing the value of Disk 1 element: " << label << std::endl;
//                    encryptor.decryptAndPrint("Part 1 Disk 1 Recovery - Before rotation",
//                                              *(cache.get(label)->getCtxt()));
                    std::cout << "rotation " << r - cache.get(label)->getRowID() << std::endl;
                    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()),
                                                          r - cache.get(label)->getRowID());
//                    encryptor.decryptAndPrint("Part 1 Disk 2 Recovery - After rotation",
//                                              *(cache.get(label)->getCtxt()));
                    recoveredShard_part1_disk1.addCtxt(*(cache.get(label)->getCtxt()));
                    ctxt.addCtxt(*(shards[colID1].getPart1Ctxt()));
                }
            }
            if (std::find(part1RecoveredRowCheck.begin(), part1RecoveredRowCheck.end(), std::to_string(r)) !=
                part1RecoveredRowCheck.end() == 0) {
                std::cout << "\n\nNote: This row/element has never recovered before\n\n";
                if (cache.contain(label1) && (ans1 == 0)) { //check if disk2 element is in Cache already
                    std::cout << "Reusing the value of Disk 2 element: " << label1 << std::endl;
//                    encryptor.decryptAndPrint("Part 2 Disk 1 Recovery - Before rotation",
//                                              *(cache.get(label1)->getCtxt()));
                    std::cout << "rotation " << r - cache.get(label1)->getRowID() << std::endl;
                    encryptor.getEncryptedArray()->rotate(*(cache.get(label1)->getCtxt()),
                                                          r - cache.get(label1)->getRowID());
//                    encryptor.decryptAndPrint("Part 2 Disk 2 Recovery - After rotation",
//                                              *(cache.get(label1)->getCtxt()));
                    ctxt.addCtxt(*(shards[colID2].getPart1Ctxt()));
                    recoveredShard_part1_disk2.addCtxt(*(cache.get(label1)->getCtxt()));

                }
            }

            if (!cache.contain(label) || !cache.contain(label1)) { //check if it is not in the cache
                helib::Ctxt ctxt(*(encryptor.getPublicKey()));
                helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
                for (int c = 1; c < cols; ++c) { // now check each col for the same row
                    colIdx1 = mod(colID1 + c, cols);
                    colIdx2 = mod(colID2 + c, cols);
                    std::cout << "\n\n-----Part 1: For  row and column: " << r << "," << c << "------" << std::endl;
                    if (r == colID1 || r == colID2) // checking if one of the faulty one is parity
                    {
//                        std::cout << "colIdx1: " << colIdx1 << std::endl;
                        if (r != colIdx1 && colIdx1 != colID1 && colIdx1 != colID2 && (ans == 0) && (ans1 == 0)) {
                            // skipping the parity and adding the good ones in ctxt
//                            std::cout << "-----Starting Inserting for " << r << "," << c << "------" << std::endl;
                            std::cout << "Inserted: " << r << "," << colIdx1 << "" << std::endl << std::endl;
                            encryptor.decryptAndPrint("FIRST TIME ctxt_before", ctxt);
                            encryptor.decryptAndPrint("ADDING COLDIX1 shardctxt", *(shards[colIdx1].getPart1Ctxt()));
                            ctxt.addCtxt(*(shards[colIdx1].getPart1Ctxt()));
                            encryptor.decryptAndPrint("AFTER ADDING IN SHARD ctxt_after", ctxt);
                            rowOperationCount++;
                            if (rowOperationCount > 2) {
                                std::cout << "Marking the workingRowFlag == True" << std::endl;
                                workingRowFlag = true;
                            }
                        }
                    } else if (cache.contain(label) && (ans == 0)) { //Checking if in Cache
                        if (r != colIdx2 && c != colIdx2) {    // skipping the parity and adding the good ones in ctxt

                            std::cout << "Cache Loop for Part1, Disk1" << std::endl;
                            std::cout << "colIdx1: " << colIdx1 << std::endl;
                            std::cout << "colIdx2: " << colIdx2 << std::endl;

                            std::cout << "--In Cache for Part1 Disk1. Inserting" << r << "," << c << "--" << std::endl;
                            std::cout << "Inserted: " << r << "," << colIdx2 << "" << std::endl << std::endl;
//                            encryptor.decryptAndPrint("ctxt_before", ctxt);
//                            encryptor.decryptAndPrint("shardctxt", *(shards[colIdx2].getPart1Ctxt()));
                            ctxt.addCtxt(*(shards[colIdx2].getPart1Ctxt()));
//                            encryptor.decryptAndPrint("ctxt_after", ctxt);
                            rowOperationCount++;
                            if (rowOperationCount > 2) {
                                std::cout << "Marking the workingRowFlag == True" << std::endl;
                                workingRowFlag = true;
                                part1RecoveredRowCheck.push_back(std::to_string(r) + "," + std::to_string(c));
                            }
                        }
                    } else if (cache.contain(label1) && (ans1 == 0)) { //Checking if in Cache
                        if (r != colIdx1 && c != colIdx1) {
                            // skipping the parity  c!=colID2 means that the recovred row should not be added again
                            // since we are using coldifx so it will generate all values except the broken one.

                            std::cout << "Cache Loop for Part1, Disk2" << std::endl;
                            std::cout << "colIdx1: " << colIdx1 << std::endl;
                            std::cout << "colIdx2: " << colIdx2 << std::endl;

                            std::cout << "-In Cache for Part1 Disk2. Inserting" << r << "," << c << "--" << std::endl;
                            std::cout << "Inserted: " << r << "," << colIdx1 << "" << std::endl << std::endl;
//                            encryptor.decryptAndPrint("ctxt_before", ctxt);
//                            encryptor.decryptAndPrint("shardctxt", *(shards[colIdx1].getPart1Ctxt()));
                            ctxt.addCtxt(*(shards[colIdx1].getPart1Ctxt()));
//                            encryptor.decryptAndPrint("ctxt_after", ctxt);
                            rowOperationCount++;
                            if (rowOperationCount > 2) {
                                std::cout << "Marking the workingRowFlag == True" << std::endl;
                                workingRowFlag = true;
                                part1RecoveredRowCheck.push_back(std::to_string(r) + "," + std::to_string(c));
                            }
                        }
                    }
                }

                //adding all the good ones are done. Now apply bitmask and XOR!
                //workingRowFlag so that masks are not added to other non working rows
                if (workingRowFlag == true) {
                    bitmask[r] = 1;
                    encryptor.decryptAndPrint("ctxt_before_bitmask", ctxt);
                    std::cout << "bitmask: " << bitmask << std::endl;
                    ctxt.multByConstant(bitmask);
                    encryptor.decryptAndPrint("ctxt_after_bitmask", ctxt);
                    std::cout << "bitmask: " << bitmask << std::endl;

                    if (r != colID1 && (ans == 0)) {
                        encryptor.decryptAndPrint("BEFORE recoveredShard_part1_disk2_adding", recoveredShard_part1_disk1);
                        std::cout << "Recovering Part 1 Disk 1 " << std::endl;
                        std::cout << "In Rotated Matrix: " << r << "," << colID1 << std::endl;
                        label = this->part_1_labelTable->get(colID1, r);
                        std::cout << "Actual Matrix (label): " << label << std::endl;
                        if (!cache.contain(label)) {
                            nodesRecoveredIter.push_back(label);
                            cache.add(label, new Panther::CacheEntry(r, colID1, new helib::Ctxt(ctxt)));
                            recoveredShard_part1_disk1.addCtxt(ctxt);
                            std::cout << "Label: " << label << " is successfully added to cache and recovered."
                                      << std::endl;
                            part1RecoveredRowCheck.push_back(std::to_string(r));
                        }

                        encryptor.decryptAndPrint("\nAFTER AFTER .... RecoveredShard_part1_disk1_adding",
                                                  recoveredShard_part1_disk1);
                    }
                    if (r != colID2 && (ans1 == 0)) {
                        encryptor.decryptAndPrint("BEFORE recoveredShard_part1_disk2adding", recoveredShard_part1_disk2);
                        std::cout << "Recovering Part 1 Disk 2 " << std::endl;
                        std::cout << "In Rotated Matrix: " << r << "," << colID2 << std::endl;
                        label = this->part_1_labelTable->get(colID2, r);
                        std::cout << "Actual Matrix (label): " << label << std::endl;
                        if (!cache.contain(label)) {
                            nodesRecoveredIter.push_back(label);
                            cache.add(label, new Panther::CacheEntry(r, colID2, new helib::Ctxt(ctxt)));
                            recoveredShard_part1_disk2.addCtxt(ctxt);
                            std::cout << "Label: " << label << " is successfully added to cache and recovered."
                                      << std::endl;
                            part1RecoveredRowCheck.push_back(std::to_string(r));
                        }
                        encryptor.decryptAndPrint("\nAFTER AFTER .... RecoveredShard_part1_disk2_adding",
                                                  recoveredShard_part1_disk2);
                    }
                }
            }


            cache.print();
            std::cout << "At the iter Part 1 end. NodesIterSize is : " << nodesRecoveredIter.size() << std::endl;
        }
        //part 2
        std::cout << "\n\n Recovering Part 2.....\n" << std::endl;
        for (int r = 0; r < rows; ++r) {
            helib::Ctxt ctxt(*(encryptor.getPublicKey()));
            helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
            workingRowFlag = false;
            rowOperationCount = 0;

            label = this->part_2_labelTable->get(colID1, r);
            label1 = this->part_2_labelTable->get(colID2, r);
            bool ans = false;
            bool ans1 = false;
            ans = (std::find(nodesRecoveredIter.begin(), nodesRecoveredIter.end(), label) != nodesRecoveredIter.end());
            ans1 = (std::find(nodesRecoveredIter.begin(), nodesRecoveredIter.end(), label1) !=
                    nodesRecoveredIter.end());
            std::cout << "ANS BOOL ANS: " << ans << std::endl;


            if (std::find(part2RecoveredRowCheck.begin(), part2RecoveredRowCheck.end(), std::to_string(r)) !=
                part2RecoveredRowCheck.end() == 0) {
                    if (cache.contain(label) && (ans == 0)) {
                        //check if disk1 element is in Cache already && !recoverd in the same iteration
                        std::cout << "Reusing the value of Disk 1 element: " << label << std::endl;
                        encryptor.decryptAndPrint("Part 2 Disk 1 Recovery - Before rotation", *(cache.get(label)->getCtxt()));
                        std::cout << "rotation " << r - cache.get(label)->getRowID() << std::endl;
                        encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), r - cache.get(label)->getRowID());
                        encryptor.decryptAndPrint("Part 1 Disk 2 Recovery - After rotation", *(cache.get(label)->getCtxt()));
                        recoveredShard_part2_disk1.addCtxt(*(cache.get(label)->getCtxt()));
                        ctxt.addCtxt(*(shards[colID1].getPart2Ctxt()));
                }
            }
            if (std::find(part2RecoveredRowCheck.begin(), part2RecoveredRowCheck.end(), std::to_string(r)) !=
                part2RecoveredRowCheck.end() == 0) {
                if (cache.contain(label1) && (ans1 == 0)) { //check if disk2 element is in Cache already
                    std::cout << "Reusing the value of Disk 2 element: " << label1 << std::endl;
                    encryptor.decryptAndPrint("Part 2 Disk 1 Recovery - Before rotation", *(cache.get(label1)->getCtxt()));
                    std::cout << "rotation " << r - cache.get(label1)->getRowID() << std::endl;
                    encryptor.getEncryptedArray()->rotate(*(cache.get(label1)->getCtxt()),
                                                          r - cache.get(label1)->getRowID());
                    encryptor.decryptAndPrint("Part 2 Disk 2 Recovery - After rotation", *(cache.get(label1)->getCtxt()));
                    ctxt.addCtxt(*(shards[colID2].getPart2Ctxt()));
                    recoveredShard_part2_disk2.addCtxt(*(cache.get(label1)->getCtxt()));
                }
            }

            if (!cache.contain(label) || !cache.contain(label1)) { //check if it is not in the cache
                helib::Ctxt ctxt(*(encryptor.getPublicKey()));
                helib::Ptxt<helib::BGV> bitmask(*(encryptor.getContext()));
                for (int c = 1; c < cols; ++c) { // now check each col for the same row
                    colIdx1 = mod(colID1 + c, cols);
                    colIdx2 = mod(colID2 + c, cols);
                    std::cout << "colIdx1: " << colIdx1 << std::endl;
                    std::cout << "colIdx2: " << colIdx2 << std::endl;

                    std::cout << "\n\n-----Part 2: For  row and column: " << r << "," << c << "------" << std::endl;
                    if (r == colID1 || r == colID2) // checking if one of the faulty one is parity
                    {
                        if (colIdx1 != rows - r - 1 && colIdx1 != colID1 && colIdx1 != colID2 && (ans == 0) &&
                            (ans1 == 0)) {
                            // skipping the parity and adding the good ones in ctxt
                            std::cout << "-----Starting Inserting for " << r << "," << c << "------" << std::endl;
                            std::cout << "Inserted: " << r << "," << colIdx1 << "" << std::endl << std::endl;
                            encryptor.decryptAndPrint("ctxt_before", ctxt);
                            encryptor.decryptAndPrint("shardctxt", *(shards[colIdx1].getPart2Ctxt()));
                            ctxt.addCtxt(*(shards[colIdx1].getPart2Ctxt()));
                            encryptor.decryptAndPrint("ctxt_after", ctxt);
                            rowOperationCount++;
                            if (rowOperationCount > 2) {
                                std::cout << "Marking the workingRowFlag == True" << std::endl;
                                workingRowFlag = true;
                            }
                        }
                    } else if (cache.contain(label) && (ans == 0)) { //Checking if in Cache
                        if (colIdx2 != rows - r - 1 &&
                            c != colIdx2) {    // skipping the parity and adding the good ones in ctxt

                            std::cout << "Cache Loop for Part1, Disk1" << std::endl;
                            std::cout << "colIdx1: " << colIdx1 << std::endl;
                            std::cout << "colIdx2: " << colIdx2 << std::endl;

                            std::cout << "--In Cache for Part1 Disk1. Inserting" << r << "," << c << "--" << std::endl;
                            std::cout << "Inserted: " << r << "," << colIdx2 << "" << std::endl << std::endl;
                            encryptor.decryptAndPrint("ctxt_before", ctxt);
                            encryptor.decryptAndPrint("shardctxt", *(shards[colIdx2].getPart2Ctxt()));
                            ctxt.addCtxt(*(shards[colIdx2].getPart2Ctxt()));
                            encryptor.decryptAndPrint("ctxt_after", ctxt);
                            rowOperationCount++;
                            if (rowOperationCount > 2) {
                                std::cout << "Marking the workingRowFlag == True" << std::endl;
                                workingRowFlag = true;
                            }
                        }
                    } else if (cache.contain(label1) && (ans1 == 0)) { //Checking if in Cache
                        if (colIdx1 != rows - r - 1 && c != colIdx1) {
                            // skipping the parity  c!=colID2 means that the recovred row should not be added again
                            // since we are using coldifx so it will generate all values except the broken one.

                            std::cout << "Cache Loop for Part2, Disk2" << std::endl;
                            std::cout << "colIdx1: " << colIdx1 << std::endl;
                            std::cout << "colIdx2: " << colIdx2 << std::endl;

                            std::cout << "-In Cache for Part2 Disk2. Inserting" << r << "," << c << "--" << std::endl;
                            std::cout << "Inserted: " << r << "," << colIdx1 << "" << std::endl << std::endl;
                            encryptor.decryptAndPrint("ctxt_before", ctxt);
                            encryptor.decryptAndPrint("shardctxt", *(shards[colIdx1].getPart2Ctxt()));
                            ctxt.addCtxt(*(shards[colIdx1].getPart2Ctxt()));
                            encryptor.decryptAndPrint("ctxt_after", ctxt);
                            rowOperationCount++;
                            if (rowOperationCount > 2) {
                                std::cout << "Marking the workingRowFlag == True" << std::endl;
                                workingRowFlag = true;
                            }
                        }
                    } else {
                        std::cout << "Two or more faulty nodes in the row!!!" << std::endl;
                    }


                }
                //adding all the good ones are done. Now apply bitmask and XOR!
                //workingRowFlag so that masks are not added to other non working rows
                if (workingRowFlag == true) {
                    bitmask[r] = 1;
                    std::cout << "bitmask: " << bitmask << std::endl;
                    ctxt.multByConstant(bitmask);
                    encryptor.decryptAndPrint("ctxt_after_bitmask", ctxt);

                    if (r != rows - 1 - colID1) {
                        encryptor.decryptAndPrint("BEFORE recoveredShard_part2_disk1 adding",
                                                  recoveredShard_part2_disk1);
                        std::cout << "Recovering Part 2 Disk 1 " << std::endl;
                        std::cout << "In Rotated Matrix: " << r << "," << colID1 << std::endl;
                        label = this->part_2_labelTable->get(colID1, r);
                        std::cout << "Actual Matrix (label): " << label << std::endl;
                        if (!cache.contain(label)) {
                            nodesRecoveredIter.push_back(label);
                            cache.add(label, new Panther::CacheEntry(r, colID1, new helib::Ctxt(ctxt)));
                            recoveredShard_part2_disk1.addCtxt(ctxt);
                            part2RecoveredRowCheck.push_back(std::to_string(r));
                            std::cout << "Label:  " << label << " is successfully added to cache and recovered."
                                      << std::endl;
                        }

                        encryptor.decryptAndPrint("\nAFTER AFTER .... RecoveredShard_part2_disk1_adding",
                                                  recoveredShard_part2_disk1);

                    }
                    if (r != rows - 1 - colID2) {
                        encryptor.decryptAndPrint("BEFORE recoveredShard_part2_disk2 adding",
                                                  recoveredShard_part2_disk2);
                        std::cout << "Recovering Part 2 Disk 2 " << std::endl;
                        std::cout << "In Rotated Matrix: " << r << "," << colID2 << std::endl;
                        label = this->part_2_labelTable->get(colID2, r);
                        std::cout << "Actual Matrix (label): " << label << std::endl;
                        if (!cache.contain(label)) {
                            nodesRecoveredIter.push_back(label);
                            cache.add(label, new Panther::CacheEntry(r, colID2, new helib::Ctxt(ctxt)));
                            recoveredShard_part2_disk2.addCtxt(ctxt);
                            part2RecoveredRowCheck.push_back(std::to_string(r));
                            std::cout << "Label:  " << label << " is successfully added to cache and recovered."
                                      << std::endl;
                        }

                        encryptor.decryptAndPrint("\nAFTER AFTER .... RecoveredShard_part2_disk2_adding",
                                                  recoveredShard_part2_disk2);
                    }


                }
            }
        }
        cache.print();
        std::cout << "Nodes added in this iteration so far .... " << std::endl;
        for (int i = 0; i < nodesRecoveredIter.size(); i++)
            std::cout << nodesRecoveredIter.at(i) << ' ' << std::endl;
        iter++;

    }
    util::debug("populating missing values...");
    label = this->part_1_labelTable->get(colID1, colID1);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << colID1 - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), colID1 - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part1_disk1.addCtxt(*(cache.get(label)->getCtxt()));
    encryptor.decryptAndPrint("\n\nRecoveredShard Part1 Disk1", recoveredShard_part1_disk1);

    label = this->part_1_labelTable->get(colID2, colID2);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << colID2 - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), colID2 - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part1_disk2.addCtxt(*(cache.get(label)->getCtxt()));
    encryptor.decryptAndPrint("\n\nRecovered Shard Part1 Disk 2", recoveredShard_part1_disk2);


    label = this->part_2_labelTable->get(colID1, rows - colID1 - 1);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << colID1 - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), colID1 - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part2_disk1.addCtxt(*(cache.get(label)->getCtxt()));
    encryptor.decryptAndPrint("\n\nRecoveredShard Part2 Disk1", recoveredShard_part2_disk1);


    label = this->part_2_labelTable->get(colID2, rows - colID2 - 1);
    std::cout << "label: " << label << std::endl;
    encryptor.decryptAndPrint("before rotation", *(cache.get(label)->getCtxt()));
    std::cout << "rotation " << colID2 - cache.get(label)->getRowID() << std::endl;
    encryptor.getEncryptedArray()->rotate(*(cache.get(label)->getCtxt()), colID2 - cache.get(label)->getRowID());
    encryptor.decryptAndPrint("after rotation", *(cache.get(label)->getCtxt()));
    recoveredShard_part2_disk2.addCtxt(*(cache.get(label)->getCtxt()));
    encryptor.decryptAndPrint("\n\nRecoveredShard Part2 Disk2", recoveredShard_part2_disk2);

}


template
class Panther::XCode<bool>;