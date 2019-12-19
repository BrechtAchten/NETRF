/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.netrf.website;

import java.util.List;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.bind.annotation.RestController;
import com.netrf.website.ThingRepository;
import com.netrf.website.ValueRepository;
import java.util.Optional;
import org.springframework.web.bind.annotation.RequestBody;

/**
 *
 * @author Brecht
 */
@RestController
@RequestMapping("")
public class IOTResource{
    @Autowired
    private ThingRepository thingRepository;
    @Autowired
    private ValueRepository valueRepository;
    
    @RequestMapping(value="/thing", method = RequestMethod.GET)
    public @ResponseBody List<Thing> getThings(){
        return thingRepository.findAll();
    }
    
    @RequestMapping(value="/thing", method = RequestMethod.POST)
    public @ResponseBody Long saveThing(@RequestBody Thing thing){
        return thingRepository.save(thing).getId();
    }
    
    @RequestMapping(value="/thing", method=RequestMethod.PUT)
    public void updateThing(@RequestBody Thing thing){
        thingRepository.save(thing);
    }
    
    @RequestMapping(value = "/thing/{id}", method = RequestMethod.GET)
    public @ResponseBody Thing getThing(@PathVariable long id){
        return thingRepository.findById(id).orElse(null);
    }
    
    @RequestMapping(value = "/thing/{thingId}/value", method = RequestMethod.GET)
    public @ResponseBody List<Value> getValues(@PathVariable long thingId){
        return valueRepository.findAll(thingId);
    }
    
    @RequestMapping(value = "/thing/{thingId}/value", method = RequestMethod.POST)
    public @ResponseBody Long saveValue(@PathVariable long thingId, @RequestBody Value value){
        Thing thing = thingRepository.findById(thingId).orElse(null);
        if(thing != null){
            thing.setLastValue(value.getValue());
            thingRepository.save(thing);
        }
        
        return valueRepository.save(value).getId();
    }
    
    @RequestMapping(value = "/thing/{thingId}/value/{from}/{to}", method = RequestMethod.GET)
    public @ResponseBody List<Value> getValues(@PathVariable long thingId, @PathVariable long from, @PathVariable long to){
        return valueRepository.findByTime(thingId, from, to);
    }
}
