/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.netrf.website;

import java.util.List;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

/**
 *
 * @author Brecht
 */
public interface ValueRepository extends JpaRepository<Value, Long>{
    @Query(value = "Select * FROM values where thingId = :thingId", nativeQuery = true)
    public List<Value> findAll(@Param("thingId") long thingId);
    
    @Query(value = "Select * FROM values where thingId = :thingId and time >= :from and time <= :to", nativeQuery = true)
    public List<Value> findByTime(@Param("thingId") long thingId, @Param("from") long from, @Param("to") long to);
}
